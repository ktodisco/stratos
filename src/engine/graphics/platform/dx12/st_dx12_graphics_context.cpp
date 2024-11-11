/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics_context.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <core/st_core.h>

#include <graphics/platform/dx12/st_dx12_conversion.h>
#include <graphics/platform/dx12/st_dx12_framebuffer.h>

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

#include <system/st_window.h>

#include <cassert>
#include <cstdio>
#include <vector>

#include <d3dcompiler.h>

extern char g_root_path[256];

st_dx12_graphics_context::st_dx12_graphics_context(const st_window* window)
{
	UINT dxgi_factory_flags = 0;
#if defined(_DEBUG)
	// Enable the D3D12 debug layer.
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debug_controller;
		if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&debug_controller)))
		{
			debug_controller->EnableDebugLayer();

			// Enable additional debug layers.
			dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	HRESULT result = CreateDXGIFactory2(dxgi_factory_flags, __uuidof(IDXGIFactory4), (void**)&factory);

	if (result != S_OK)
	{
		assert(false);
	}

	// Enumerate the adapters in the machine.
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (uint32_t adapter_itr = 0;
		factory->EnumAdapters1(adapter_itr, &adapter) != DXGI_ERROR_NOT_FOUND;
		++adapter_itr)
	{
		adapters.push_back(adapter);
	}

	// Choose the adapter with the most dedicated memory.
	Microsoft::WRL::ComPtr<IDXGIAdapter1> hardware_adapter;
	size_t max_memory = 0;
	for (uint32_t adapter_itr = 0; adapter_itr < adapters.size(); ++adapter_itr)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapters[adapter_itr]->GetDesc1(&desc);

		if (desc.DedicatedVideoMemory > max_memory);
		{
			hardware_adapter = adapters[adapter_itr];
			max_memory = desc.DedicatedVideoMemory;

			// Naively take the first one. At the time of development, this is commonly
			// the dedicated graphics card.
			break;
		}
	}

	// Create the device and command queue.
	result = D3D12CreateDevice(
		hardware_adapter.Get(),
		D3D_FEATURE_LEVEL_12_1,
		__uuidof(ID3D12Device),
		(void**)&_device);

	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = _device->CreateCommandQueue(
		&queue_desc,
		__uuidof(ID3D12CommandQueue),
		(void**)&_command_queue);

	if (result != S_OK)
	{
		assert(false);
	}

	// Create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
	swap_chain_desc.BufferCount = k_max_frames;
	swap_chain_desc.Width = window->get_width();
	swap_chain_desc.Height = window->get_height();
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain;
	result = factory->CreateSwapChainForHwnd(
		_command_queue.Get(),
		window->get_window_handle(),
		&swap_chain_desc,
		nullptr,
		nullptr,
		&swap_chain);

	if (result != S_OK)
	{
		assert(false);
	}

	// Convert the swap chain to IDXGISwapChain3.
	swap_chain.As(&_swap_chain);

	if (result != S_OK)
	{
		assert(false);
	}

	// Create descriptor heaps.
	_resource_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		k_max_loaded_resources,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// TODO: The render target count could either be more dynamic, or defined better.
	_rtv_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		k_max_frames + 16,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	_dsv_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		16,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	_sampler_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		k_max_samplers,
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	for (uint32_t h_itr = 0; h_itr < k_max_frames; ++h_itr)
	{
		_cbv_srv_heap[h_itr] = std::make_unique<st_dx12_descriptor_heap>(
			_device.Get(),
			k_max_shader_resources,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	// Create the descriptor heap for imgui.
	_gui_srv_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		1,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	// Create the data upload heap. Let's just make it 128K for now.
	create_buffer_internal(128 * 1024 * 1024, _upload_buffer.GetAddressOf());
	
	// Map the upload buffer.
	D3D12_RANGE map_range{0, 0};
	_upload_buffer->Map(0, &map_range, reinterpret_cast<void**>(&_upload_buffer_head));

	// Create frame resources.
	for (uint32_t rtv_itr = 0; rtv_itr < k_max_frames; ++rtv_itr)
	{
		result = _swap_chain->GetBuffer(
			rtv_itr,
			__uuidof(ID3D12Resource),
			(void**)&_backbuffers[rtv_itr]);

		ST_NAME_DX12_OBJECT(_backbuffers[rtv_itr], str_to_wstr("Backbuffer").c_str());

		if (result != S_OK)
		{
			assert(false);
		}

		st_dx12_cpu_descriptor_handle rtv_handle = _rtv_heap->allocate_handle();
		_device->CreateRenderTargetView(_backbuffers[rtv_itr].Get(), nullptr, rtv_handle._handle);
	}

	for (uint32_t ca_itr = 0; ca_itr < k_max_frames; ++ca_itr)
	{
		// Create the command allocator.
		result = _device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			__uuidof(ID3D12CommandAllocator),
			(void**)&_command_allocators[ca_itr]);
	}

	if (result != S_OK)
	{
		assert(false);
	}

	// Create the command list.
	result = _device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		_command_allocators[0].Get(),
		nullptr,
		__uuidof(ID3D12GraphicsCommandList),
		(void**)&_command_list);

	if (result != S_OK)
	{
		assert(false);
	}

	// The command list starts open, so close it first.
	_command_list->Close();

	// Create a default root signature.
	D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {};
	feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data)) != S_OK)
	{
		feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	CD3DX12_DESCRIPTOR_RANGE1 ranges[4];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 8, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8, 0);

	CD3DX12_ROOT_PARAMETER1 root_parameters[4];
	root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	root_parameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
	root_parameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_ALL);
	root_parameters[3].InitAsDescriptorTable(1, &ranges[3], D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
	root_signature_desc.Init_1_1(
		_countof(root_parameters),
		root_parameters,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	result = D3DX12SerializeVersionedRootSignature(
		&root_signature_desc,
		feature_data.HighestVersion,
		&signature,
		&error);

	if (result != S_OK)
	{
		assert(false);
	}

	result = _device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		__uuidof(ID3D12RootSignature),
		(void**)&_root_signature);

	if (result != S_OK)
	{
		assert(false);
	}

	// Create synchronization objects.
	result = _device->CreateFence(_fence_value, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&_fence);
	_fence_value++;

	if (result != S_OK)
	{
		assert(false);
	}

	_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (_fence_event == nullptr)
	{
		if (GetLastError() != S_OK)
		{
			assert(false);
		}
	}

	// Set up the dynamic geometry buffers.
	// TODO: Better way of managing this constant.
	const size_t k_dynamic_buffer_size = 16 * 1000 * 1000;
	create_buffer_internal(k_dynamic_buffer_size, _dynamic_vertex_buffer.GetAddressOf());
	create_buffer_internal(k_dynamic_buffer_size, _dynamic_index_buffer.GetAddressOf());

	begin_frame();

	// Create the faux backbuffer target.
	_present_target = std::make_unique<st_render_texture>(
		this,
		window->get_width(),
		window->get_height(),
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::color_target | e_st_texture_usage::copy_source,
		st_texture_state_copy_source,
		st_vec4f{ 0.0f, 0.0f, 0.0f, 1.0f },
		"Present Target");
}

st_dx12_graphics_context::~st_dx12_graphics_context()
{
	_present_target = nullptr;

	_dynamic_index_buffer = nullptr;
	_dynamic_vertex_buffer = nullptr;

	_fence = nullptr;
	_root_signature = nullptr;
	_command_list = nullptr;

	for (uint32_t ca_itr = 0; ca_itr < k_max_frames; ++ca_itr)
	{
		_command_allocators[ca_itr] = nullptr;
	}

	_upload_buffer = nullptr;

	_gui_srv_heap = nullptr;
	_sampler_heap = nullptr;
	_dsv_heap = nullptr;
	_rtv_heap = nullptr;
	_resource_heap = nullptr;
	for (uint32_t h_itr = 0; h_itr < k_max_frames; ++h_itr)
	{
		_cbv_srv_heap[h_itr] = nullptr;
	}

	for (uint32_t bb_itr = 0; bb_itr < k_max_frames; ++bb_itr)
	{
		_backbuffers[bb_itr] = nullptr;
	}

	_swap_chain = nullptr;
	_command_queue = nullptr;
	_device = nullptr;

#if defined(_DEBUG)
	// Enable the D3D12 debug layer.
	{
		HMODULE debug_module = GetModuleHandle("Dxgidebug.dll");
		if (debug_module)
		{
			typedef HRESULT (WINAPI* LPDXGIGETDEBUGINTERFACE)(REFIID, void**);
			auto dxgiGetDebugInterface =
				reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(GetProcAddress(debug_module, "DXGIGetDebugInterface"));

			Microsoft::WRL::ComPtr<IDXGIDebug> dxgi_debug;
			if (SUCCEEDED(dxgiGetDebugInterface(__uuidof(IDXGIDebug), (void**)&dxgi_debug)))
			{
				dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			}
		}
	}
#endif
}

void st_dx12_graphics_context::acquire()
{
}

void st_dx12_graphics_context::release()
{
}

void st_dx12_graphics_context::set_pipeline(const st_pipeline* _state)
{
	const st_dx12_pipeline* state = static_cast<const st_dx12_pipeline*>(_state);
	_command_list->SetPipelineState(state->_pipeline.Get());
}

void st_dx12_graphics_context::set_viewport(const st_viewport& viewport)
{
	D3D12_VIEWPORT v;
	v.TopLeftX = viewport._x;
	v.TopLeftY = viewport._y;
	v.Width = viewport._width;
	v.Height = viewport._height;
	v.MinDepth = viewport._min_depth;
	v.MaxDepth = viewport._max_depth;
	_command_list->RSSetViewports(1, &v);
}

void st_dx12_graphics_context::set_scissor(int left, int top, int right, int bottom)
{
	D3D12_RECT rect;
	rect.left = left;
	rect.right = right;
	rect.top = top;
	rect.bottom = bottom;

	_command_list->RSSetScissorRects(1, &rect);
}

void st_dx12_graphics_context::set_clear_color(float r, float g, float b, float a)
{
	_clear_color[0] = r;
	_clear_color[1] = g;
	_clear_color[2] = b;
	_clear_color[3] = a;
}

void st_dx12_graphics_context::set_blend_factor(float r, float g, float b, float a)
{
	const float factor[] = { r, g, b, a };
	_command_list->OMSetBlendFactor(factor);
}

void st_dx12_graphics_context::set_shader_resource_table(uint32_t offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE srv_handle = _cbv_srv_heap[_frame_index]->get_handle_gpu(offset);
	_command_list->SetGraphicsRootDescriptorTable(st_descriptor_slot_textures, srv_handle);
}

void st_dx12_graphics_context::set_sampler_table(uint32_t offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE sampler_handle = _sampler_heap->get_handle_gpu(offset);
	_command_list->SetGraphicsRootDescriptorTable(st_descriptor_slot_samplers, sampler_handle);
}

void st_dx12_graphics_context::set_constant_buffer_table(uint32_t offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbv_handle = _cbv_srv_heap[_frame_index]->get_handle_gpu(offset);
	_command_list->SetGraphicsRootDescriptorTable(st_descriptor_slot_constants, cbv_handle);
}

void st_dx12_graphics_context::set_buffer_table(uint32_t offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE buffer_handle = _cbv_srv_heap[_frame_index]->get_handle_gpu(offset);
	_command_list->SetGraphicsRootDescriptorTable(st_descriptor_slot_buffers, buffer_handle);
}

void st_dx12_graphics_context::set_render_targets(
	uint32_t count,
	const st_texture_view** targets,
	const st_texture_view* depth_stencil)
{
	for (uint32_t target_itr = 0; target_itr < count; ++target_itr)
	{
		const st_dx12_texture_view* texture_view = static_cast<const st_dx12_texture_view*>(targets[target_itr]);
		D3D12_CPU_DESCRIPTOR_HANDLE target_handle = _rtv_heap->get_handle_cpu(texture_view->_handle);
		_bound_targets[target_itr] = target_handle;
	}

	// Clear out stale render targets.
	for (uint32_t target_itr = count; target_itr < 8; ++target_itr)
	{
		_bound_targets[target_itr] = {};
	}

	if (depth_stencil)
	{
		const st_dx12_texture_view* ds_view = static_cast<const st_dx12_texture_view*>(depth_stencil);
		D3D12_CPU_DESCRIPTOR_HANDLE depth_handle = _dsv_heap->get_handle_cpu(ds_view->_handle);
		_bound_depth_stencil = depth_handle;
	}

	_command_list->OMSetRenderTargets(
		count,
		_bound_targets,
		false,
		depth_stencil ? &_bound_depth_stencil : nullptr);
}

void st_dx12_graphics_context::clear(unsigned int clear_flags)
{
	if (clear_flags & st_clear_flag_color)
	{
		for (uint32_t target_itr = 0; target_itr < 8; ++target_itr)
		{
			if (_bound_targets[target_itr].ptr)
			{
				_command_list->ClearRenderTargetView(
					_bound_targets[target_itr],
					_clear_color,
					0,
					nullptr);
			}
		}
	}

	if (clear_flags & st_clear_flag_depth)
	{
		_command_list->ClearDepthStencilView(
			_bound_depth_stencil,
			D3D12_CLEAR_FLAG_DEPTH,
			1.0f,
			0,
			0,
			nullptr);
	}

	if (clear_flags & st_clear_flag_stencil)
	{
		_command_list->ClearDepthStencilView(
			_bound_depth_stencil,
			D3D12_CLEAR_FLAG_STENCIL,
			0.0f,
			0,
			0,
			nullptr);
	}
}

void st_dx12_graphics_context::draw(const st_static_drawcall& drawcall)
{
	const st_dx12_buffer* vertex = static_cast<const st_dx12_buffer*>(drawcall._vertex_buffer);
	const st_dx12_buffer* index = static_cast<const st_dx12_buffer*>(drawcall._index_buffer);

	D3D12_VERTEX_BUFFER_VIEW vertex_view;
	vertex_view.BufferLocation = vertex->_buffer->GetGPUVirtualAddress();
	vertex_view.SizeInBytes = vertex->_element_size * vertex->_count;
	vertex_view.StrideInBytes = vertex->_element_size;

	D3D12_INDEX_BUFFER_VIEW index_view;
	index_view.BufferLocation = index->_buffer->GetGPUVirtualAddress();
	index_view.SizeInBytes = index->_element_size * index->_count;
	index_view.Format = DXGI_FORMAT_R16_UINT;

	_command_list->IASetPrimitiveTopology(convert_topology(drawcall._draw_mode));
	_command_list->IASetVertexBuffers(0, 1, &vertex_view);
	_command_list->IASetIndexBuffer(&index_view);
	_command_list->DrawIndexedInstanced(drawcall._index_count, 1, drawcall._index_offset, drawcall._vertex_offset, 0);
}

void st_dx12_graphics_context::draw(const st_dynamic_drawcall& drawcall)
{
	struct st_procedural_vertex
	{
		st_vec3f _pos;
		st_vec3f _color;
	};

	// TODO: Dynamic buffer limit checking.

	uint8_t* buffer_begin;
	D3D12_RANGE range = { 0, 0 };
	HRESULT result = _dynamic_vertex_buffer->Map(0, &range, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_vertex_bytes_written;

	st_dx12_buffer_view dynamic_vertex_buffer_view;
	dynamic_vertex_buffer_view.vertex.BufferLocation = _dynamic_vertex_buffer->GetGPUVirtualAddress() + _dynamic_vertex_bytes_written;
	dynamic_vertex_buffer_view.vertex.StrideInBytes = sizeof(st_procedural_vertex);
	dynamic_vertex_buffer_view.vertex.SizeInBytes = drawcall._positions.size() * sizeof(st_procedural_vertex);

	if (result != S_OK)
	{
		assert(false);
	}

	std::vector<st_procedural_vertex> verts;
	verts.reserve(drawcall._positions.size());

	for (uint32_t vert_itr = 0; vert_itr < drawcall._positions.size(); ++vert_itr)
	{
		verts.push_back({ drawcall._positions[vert_itr], drawcall._colors[vert_itr] });
	}

	memcpy(buffer_begin, &verts[0], sizeof(st_procedural_vertex) * verts.size());
	_dynamic_vertex_buffer->Unmap(0, nullptr);
	_dynamic_vertex_bytes_written += sizeof(st_procedural_vertex) * verts.size();

	range.Begin = _dynamic_index_bytes_written;
	result = _dynamic_index_buffer->Map(0, &range, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_index_bytes_written;

	st_dx12_buffer_view dynamic_index_buffer_view;
	dynamic_index_buffer_view.index.BufferLocation = _dynamic_index_buffer->GetGPUVirtualAddress() + _dynamic_index_bytes_written;
	dynamic_index_buffer_view.index.Format = DXGI_FORMAT_R16_UINT;
	dynamic_index_buffer_view.index.SizeInBytes = sizeof(uint16_t) * drawcall._indices.size();

	if (result != S_OK)
	{
		assert(false);
	}

	memcpy(buffer_begin, &drawcall._indices[0], sizeof(uint16_t) * drawcall._indices.size());
	_dynamic_index_buffer->Unmap(0, nullptr);
	_dynamic_index_bytes_written += sizeof(uint16_t) * drawcall._indices.size();

	_command_list->IASetPrimitiveTopology(convert_topology(drawcall._draw_mode));
	_command_list->IASetVertexBuffers(0, 1, &dynamic_vertex_buffer_view.vertex);
	_command_list->IASetIndexBuffer(&dynamic_index_buffer_view.index);
	_command_list->DrawIndexedInstanced(drawcall._indices.size(), 1, 0, 0, 0);
}

st_render_texture* st_dx12_graphics_context::get_present_target() const
{
	return _present_target.get();
}

void st_dx12_graphics_context::transition_backbuffer_to_target()
{
	transition(_present_target->get_texture(), st_texture_state_render_target);
}

void st_dx12_graphics_context::transition_backbuffer_to_present()
{
	transition(_present_target->get_texture(), st_texture_state_copy_source);
}

std::unique_ptr<st_texture> st_dx12_graphics_context::create_texture(const st_texture_desc& desc)
{
	std::unique_ptr<st_dx12_texture> texture = std::make_unique<st_dx12_texture>();
	texture->_width = desc._width;
	texture->_height = desc._height;
	texture->_levels = desc._levels;
	texture->_format = desc._format;
	texture->_usage = desc._usage;

	DXGI_FORMAT dx_format = convert_format(desc._format);

	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
	if (desc._usage & e_st_texture_usage::color_target) flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (desc._usage & e_st_texture_usage::depth_target) flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	if (desc._usage & e_st_texture_usage::storage) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_RESOURCE_DESC texture_desc{};
	texture_desc.MipLevels = desc._levels;
	texture_desc.Format = dx_format;
	texture_desc.Width = desc._width;
	texture_desc.Height = desc._height;
	texture_desc.Flags = flags;
	texture_desc.DepthOrArraySize = 1;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	bool is_target = false;
	D3D12_CLEAR_VALUE clear_value = {};
	if (desc._usage & e_st_texture_usage::color_target ||
		desc._usage & e_st_texture_usage::depth_target)
	{
		is_target = true;
		clear_value.Format = dx_format;

		if (desc._format == st_format_d24_unorm_s8_uint)
		{
			clear_value.DepthStencil.Depth = desc._clear._depth_stencil._depth;
			clear_value.DepthStencil.Stencil = desc._clear._depth_stencil._stencil;
		}
		else
		{
			clear_value.Color[0] = desc._clear._color.x;
			clear_value.Color[1] = desc._clear._color.y;
			clear_value.Color[2] = desc._clear._color.z;
			clear_value.Color[3] = desc._clear._color.w;
		}
	}

	D3D12_HEAP_PROPERTIES heap_properties{
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		1,
		1
	};

	HRESULT result = _device->CreateCommittedResource(
		&heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&texture_desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		is_target ? &clear_value : nullptr,
		__uuidof(ID3D12Resource),
		(void**)texture->_handle.GetAddressOf());

	if (result != S_OK)
	{
		assert(false);
	}

	if (desc._data)
	{
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		uint8_t* bits = reinterpret_cast<uint8_t*>(desc._data);
		for (uint32_t level = 0; level < desc._levels; ++level)
		{
			uint32_t level_width = desc._width >> level;
			uint32_t level_height = desc._height >> level;

			size_t row_bytes;
			size_t num_bytes;
			get_surface_info(
				level_width,
				level_height,
				desc._format,
				&num_bytes,
				&row_bytes,
				nullptr);

			D3D12_SUBRESOURCE_DATA res =
			{
				bits,
				static_cast<LONG_PTR>(row_bytes),
				static_cast<LONG_PTR>(num_bytes)
			};

			subresources.push_back(res);

			bits += num_bytes;
		}

		size_t alloc_size = (sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(uint32_t) + sizeof(uint64_t)) * desc._levels;
		auto layouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(malloc(alloc_size));

		uint64_t* row_sizes_bytes = reinterpret_cast<uint64_t*>(layouts + desc._levels);
		uint32_t* row_count = reinterpret_cast<uint32_t*>(row_sizes_bytes + desc._levels);
		uint64_t required_size = 0;
		_device->GetCopyableFootprints(&texture_desc, 0, desc._levels, 0, layouts, row_count, row_sizes_bytes, &required_size);

		_upload_buffer_offset = align_value(_upload_buffer_offset, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
		size_t initial_offset = _upload_buffer_offset;

		for (uint32_t i = 0; i < desc._levels; ++i)
		{
			if (row_sizes_bytes[i] > size_t(-1))
			{
				assert(false);
			}

			D3D12_MEMCPY_DEST DestData =
			{
				reinterpret_cast<uint8_t*>(_upload_buffer_head) + _upload_buffer_offset,
				layouts[i].Footprint.RowPitch,
				size_t(layouts[i].Footprint.RowPitch) * size_t(row_count[i])
			};

			MemcpySubresource(&DestData, &subresources[i], static_cast<size_t>(row_sizes_bytes[i]), row_count[i], layouts[i].Footprint.Depth);

			_upload_buffer_offset += (layouts[i].Footprint.RowPitch * layouts[i].Footprint.Height * layouts[i].Footprint.Depth) / 4;

			// The offset of the layout needs to be adjusted by the amount we've written into the upload buffer
			// to this point in the frame, for when it's used as a copy location below.
			layouts[i].Offset += initial_offset;
		}

		for (uint32_t i = 0; i < desc._levels; ++i)
		{
			// Copy the upload heap to the texture 2D.
			D3D12_TEXTURE_COPY_LOCATION dest_location
			{
				*texture->_handle.GetAddressOf(),
				D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
				i
			};

			D3D12_TEXTURE_COPY_LOCATION src_location
			{
				_upload_buffer.Get(),
				D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
				layouts[i]
			};

			_command_list->CopyTextureRegion(
				&dest_location,
				0,
				0,
				0,
				&src_location,
				nullptr);
		}

		free(layouts);
	}

	_command_list->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			*texture->_handle.GetAddressOf(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			convert_resource_state(desc._initial_state)));

	texture->_state = desc._initial_state;

	return std::move(texture);
}

void st_dx12_graphics_context::set_texture_meta(st_texture* texture, const char* name)
{
}

void st_dx12_graphics_context::set_texture_name(st_texture* texture_, std::string name)
{
	st_dx12_texture* texture = static_cast<st_dx12_texture*>(texture_);
	ST_NAME_DX12_OBJECT(texture->_handle.Get(), str_to_wstr(name).c_str());
}

void st_dx12_graphics_context::transition(
	st_texture* texture_,
	e_st_texture_state new_state)
{
	st_dx12_texture* texture = static_cast<st_dx12_texture*>(texture_);
	e_st_texture_state old_state = texture->_state;

	if (old_state == new_state)
		return;

	// TODO: It's bad practice to transition one at a time. These should be accumulated
	// and then flushed all at once.
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		texture->_handle.Get(),
		convert_resource_state(old_state),
		convert_resource_state(new_state));

	texture->_state = new_state;

	_command_list->ResourceBarrier(1, &barrier);
}

// TODO: Rewrite this to replace create_shader_resource_view.
std::unique_ptr<st_texture_view> st_dx12_graphics_context::create_texture_view(st_texture* texture_)
{
	st_dx12_texture* texture = static_cast<st_dx12_texture*>(texture_);

	std::unique_ptr<st_dx12_texture_view> texture_view = std::make_unique<st_dx12_texture_view>();

	if (texture->_usage & e_st_texture_usage::depth_target)
	{
		// Create the depth/stencil view.
		st_dx12_cpu_descriptor_handle dsv_handle = _dsv_heap->allocate_handle();
		_device->CreateDepthStencilView(*texture->_handle.GetAddressOf(), nullptr, dsv_handle._handle);

		texture_view->_handle = dsv_handle._offset;
	}
	else if (texture->_usage & e_st_texture_usage::color_target)
	{
		// Create the render target view.
		st_dx12_cpu_descriptor_handle rtv_handle = _rtv_heap->allocate_handle();
		_device->CreateRenderTargetView(*texture->_handle.GetAddressOf(), nullptr, rtv_handle._handle);

		texture_view->_handle = rtv_handle._offset;
	}
	else
	{
		// Create the shader resource view.
		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.Format = convert_format(texture->_format);
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = texture->_levels;

		st_dx12_cpu_descriptor_handle srv_handle = _resource_heap->allocate_handle();
		_device->CreateShaderResourceView(texture->_handle.Get(), &srv_desc, srv_handle._handle);

		texture_view->_handle = srv_handle._offset;
	}

	return std::move(texture_view);
}

std::unique_ptr<st_sampler> st_dx12_graphics_context::create_sampler(const st_sampler_desc& desc)
{
	D3D12_SAMPLER_DESC sampler_desc = {};

	// The dx12 filter enum is organized so all point filters are 0s and linear
	// filters are always the same bit depending on category.
	uint32_t filter = 0x0;
	if (desc._mip_filter == st_filter_linear)
		filter |= 0x1;
	if (desc._mag_filter == st_filter_linear)
		filter |= 0x4;
	if (desc._min_filter == st_filter_linear)
		filter |= 0x10;

	sampler_desc.Filter = (D3D12_FILTER)filter;

	sampler_desc.AddressU = convert_address_mode(desc._address_u);
	sampler_desc.AddressV = convert_address_mode(desc._address_v);
	sampler_desc.AddressW = convert_address_mode(desc._address_w);
	sampler_desc.MinLOD = desc._min_mip;
	sampler_desc.MaxLOD = desc._max_mip;
	sampler_desc.MipLODBias = desc._mip_bias;
	sampler_desc.MaxAnisotropy = desc._anisotropy;
	sampler_desc.ComparisonFunc = convert_comparison_func(desc._compare_func);

	st_dx12_cpu_descriptor_handle sampler_handle = _sampler_heap->allocate_handle();
	_device->CreateSampler(&sampler_desc, sampler_handle._handle);

	std::unique_ptr<st_dx12_sampler> sampler = std::make_unique<st_dx12_sampler>();
	sampler->_handle = sampler_handle._offset;

	return std::move(sampler);
}

std::unique_ptr<st_buffer> st_dx12_graphics_context::create_buffer(const st_buffer_desc& desc)
{
	std::unique_ptr<st_dx12_buffer> buffer = std::make_unique<st_dx12_buffer>();
	buffer->_count = desc._count;
	buffer->_element_size = desc._element_size;
	buffer->_usage = desc._usage;

	// Structured buffers in DX12 are recommended to be 128-byte aligned.
	const uint32_t k_min_buffer_size = 128;
	// Constant buffers in DX12 must be 256-byte aligned.
	const uint32_t k_min_cb_size = 256;

	uint32_t min_size = k_min_buffer_size;
	if (desc._usage & e_st_buffer_usage::uniform)
		min_size = k_min_cb_size;

	// We store the original size so that the memcpy in update does not overread
	// the bounds of the passed memory.
	create_buffer_internal(
		align_value(buffer->_count * buffer->_element_size, min_size),
		buffer->_buffer.GetAddressOf());

	return std::move(buffer);
}

void st_dx12_graphics_context::create_buffer_internal(size_t size, ID3D12Resource** resource)
{
	D3D12_HEAP_PROPERTIES heap_properties{
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		1,
		1
	};

	D3D12_RESOURCE_DESC buffer_desc{};
	buffer_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	buffer_desc.Width = size;
	buffer_desc.Height = 1;
	buffer_desc.DepthOrArraySize = 1;
	buffer_desc.MipLevels = 1;
	buffer_desc.Format = DXGI_FORMAT_UNKNOWN;
	buffer_desc.SampleDesc.Count = 1;
	buffer_desc.SampleDesc.Quality = 0;
	buffer_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	buffer_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	HRESULT result = _device->CreateCommittedResource(
		&heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&buffer_desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		__uuidof(ID3D12Resource),
		(void**)resource);

	if (result != S_OK)
	{
		assert(false);
	}
}

std::unique_ptr<st_buffer_view> st_dx12_graphics_context::create_buffer_view(st_buffer* buffer_)
{
	st_dx12_buffer* buffer = static_cast<st_dx12_buffer*>(buffer_);

	std::unique_ptr<st_dx12_buffer_view> buffer_view = std::make_unique<st_dx12_buffer_view>();
	if (buffer->_usage & e_st_buffer_usage::vertex)
	{
		buffer_view->vertex.BufferLocation = buffer->_buffer->GetGPUVirtualAddress();
		buffer_view->vertex.StrideInBytes = buffer->_element_size;
		buffer_view->vertex.SizeInBytes = buffer->_element_size * buffer->_count;
	}
	else if (buffer->_usage & e_st_buffer_usage::index)
	{
		buffer_view->index.BufferLocation = buffer->_buffer->GetGPUVirtualAddress();
		buffer_view->index.SizeInBytes = buffer->_element_size * buffer->_count;
		buffer_view->index.Format = DXGI_FORMAT_R16_UINT;
	}
	else
	{
		// TODO.
	}

	return std::move(buffer_view);
}

void st_dx12_graphics_context::map(
	st_buffer* buffer_,
	uint32_t subresource,
	const st_range& range_,
	void** outData)
{
	st_dx12_buffer* buffer = static_cast<st_dx12_buffer*>(buffer_);

	D3D12_RANGE range { range_.begin, range_.end };
	HRESULT result = buffer->_buffer->Map(subresource, &range, outData);

	if (result != S_OK)
	{
		assert(false);
	}
}

void st_dx12_graphics_context::unmap(st_buffer* buffer_, uint32_t subresource, const st_range& range_)
{
	st_dx12_buffer* buffer = static_cast<st_dx12_buffer*>(buffer_);

	D3D12_RANGE range{ range_.begin, range_.end };
	buffer->_buffer->Unmap(subresource, &range);
}

void st_dx12_graphics_context::update_buffer(st_buffer* buffer_, void* data, const uint32_t offset, const uint32_t count)
{
	st_dx12_buffer* buffer = static_cast<st_dx12_buffer*>(buffer_);

	uint8_t* head;
	map(buffer_, 0, { 0, 0 }, (void**)&head);  
	memcpy(head + offset, data, count * buffer->_element_size);
	unmap(buffer_, 0, { 0, 0 });
}

void st_dx12_graphics_context::set_buffer_meta(st_buffer* buffer, std::string name)
{
}

void st_dx12_graphics_context::add_constant(
	st_buffer* buffer,
	const std::string& name,
	const e_st_shader_constant_type constant_type)
{
}

std::unique_ptr<st_resource_table> st_dx12_graphics_context::create_resource_table()
{
	std::unique_ptr<st_dx12_resource_table> table = std::make_unique<st_dx12_resource_table>();
	return std::move(table);
}

void st_dx12_graphics_context::set_constant_buffers(
	st_resource_table* _table,
	uint32_t count,
	st_buffer** cbs)
{
	st_dx12_resource_table* table = static_cast<st_dx12_resource_table*>(_table);

	for (uint32_t i = 0; i < count; ++i)
	{
		st_dx12_buffer* cb = static_cast<st_dx12_buffer*>(cbs[i]);

		st_dx12_descriptor cbv = create_constant_buffer_view(
			cb->_buffer->GetGPUVirtualAddress(),
			cb->_element_size);
		table->_cbvs.push_back(cbv);
	}
}

void st_dx12_graphics_context::set_textures(
	st_resource_table* table_,
	uint32_t count,
	st_texture** textures,
	st_sampler** samplers)
{
	st_dx12_resource_table* table = static_cast<st_dx12_resource_table*>(table_);

	for (uint32_t i = 0; i < count; ++i)
	{
		if (textures)
		{
			st_dx12_texture* sr = static_cast<st_dx12_texture*>(textures[i]);

			// TODO: Depending on whether we want depth or stencil, this needs to be flexible.
			e_st_format format = sr->_format;
			if (format == st_format_d24_unorm_s8_uint)
			{
				format = st_format_r24_unorm_x8_typeless;
			}

			st_dx12_descriptor srv = create_shader_resource_view(
				sr->_handle.Get(),
				format,
				sr->_levels);

			table->_srvs.push_back(srv);
		}
		else
		{
			table->_srvs.push_back(0);
		}

		if (samplers)
		{
			st_dx12_sampler* s = static_cast<st_dx12_sampler*>(samplers[i]);
			table->_samplers.push_back(s->_handle);
		}
		else
		{
			table->_samplers.push_back(0);
		}
	}
}

void st_dx12_graphics_context::set_buffers(
	st_resource_table* _table,
	uint32_t count,
	st_buffer** buffers)
{
	st_dx12_resource_table* table = static_cast<st_dx12_resource_table*>(_table);

	for (uint32_t i = 0; i < count; ++i)
	{
		st_dx12_buffer* buffer = static_cast<st_dx12_buffer*>(buffers[i]);

		st_dx12_descriptor srv = create_buffer_view(
			buffer->_buffer.Get(),
			buffer->_count,
			buffer->_element_size);
		table->_buffers.push_back(srv);
	}
}

void st_dx12_graphics_context::update_textures(st_resource_table* table_, uint32_t count, st_texture_view** views)
{
	st_dx12_resource_table* table = static_cast<st_dx12_resource_table*>(table_);

	for (uint32_t itr = 0; itr < count; ++itr)
	{
		st_dx12_texture_view* view = static_cast<st_dx12_texture_view*>(views[itr]);
		table->_srvs[itr] = view->_handle;
	}
}

void st_dx12_graphics_context::bind_resource_table(st_resource_table* table_)
{
	st_dx12_resource_table* table = static_cast<st_dx12_resource_table*>(table_);

	static uint32_t k_invalid_offset = 0xffffffff;

	uint32_t offset = k_invalid_offset;
	for (uint32_t cbv_itr = 0; cbv_itr < table->_cbvs.size(); ++cbv_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _cbv_srv_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->CopyDescriptorsSimple(
			1,
			handle._handle,
			_resource_heap->get_handle_cpu(table->_cbvs[cbv_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (table->_cbvs.size() > 0)
		set_constant_buffer_table(offset);

	offset = k_invalid_offset;
	for (uint32_t srv_itr = 0; srv_itr < table->_srvs.size(); ++srv_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _cbv_srv_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->CopyDescriptorsSimple(
			1,
			handle._handle,
			_resource_heap->get_handle_cpu(table->_srvs[srv_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (table->_srvs.size() > 0)
		set_shader_resource_table(offset);

	offset = k_invalid_offset;
	for (uint32_t b_itr = 0; b_itr < table->_buffers.size(); ++b_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _cbv_srv_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->CopyDescriptorsSimple(
			1,
			handle._handle,
			_resource_heap->get_handle_cpu(table->_buffers[b_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (table->_buffers.size() > 0)
		set_buffer_table(offset);

	// TODO: Samplers are still left as plain entries into the samplers table because
	// sampler usage is not very complex. This will likely need to change in the future.
	if (table->_samplers.size() > 0) { set_sampler_table(table->_samplers[0]); }
}

std::unique_ptr<st_shader> st_dx12_graphics_context::create_shader(const char* filename, uint8_t type)
{
	std::unique_ptr<st_dx12_shader> shader = std::make_unique<st_dx12_shader>();

	auto load_shader = [](std::string filename, ID3DBlob** blob)
	{
		std::wstring full_path =
			str_to_wstr(g_root_path) +
			str_to_wstr(filename) +
			str_to_wstr(".cso");

		D3DReadFileToBlob(full_path.c_str(), blob);
	};

	if (type & st_shader_type_vertex)
	{
		load_shader(std::string(filename) + std::string("_vert"), shader->_vs.GetAddressOf());
	}

	if (type & st_shader_type_pixel)
	{
		load_shader(std::string(filename) + std::string("_frag"), shader->_ps.GetAddressOf());
	}

	// TODO: Other shader types.

	shader->_type = type;

	return std::move(shader);
}

std::unique_ptr<st_pipeline> st_dx12_graphics_context::create_pipeline(const st_pipeline_state_desc& desc)
{
	std::unique_ptr<st_dx12_pipeline> pipeline = std::make_unique<st_dx12_pipeline>();

	// Construct the DX12 pipeline state description using our internal representation.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_desc = {};

	// Set the root signature from the graphics context.
	pipeline_desc.pRootSignature = _root_signature.Get();

	// Get the input layout from the vertex format.
	const st_dx12_vertex_format* vertex_format = static_cast<const st_dx12_vertex_format*>(desc._vertex_format);
	pipeline_desc.InputLayout = vertex_format->_input_layout;

	const st_dx12_shader* shader = static_cast<const st_dx12_shader*>(desc._shader);

	// Get the shader bytecodes.
	// Ordinarily I'd use ternary operators here, but that generated code which would call
	// the shader bytecode helper constructor with a null blob.
	if (shader->_type & st_shader_type_vertex) { pipeline_desc.VS = CD3DX12_SHADER_BYTECODE(shader->_vs.Get()); }
	if (shader->_type & st_shader_type_pixel) { pipeline_desc.PS = CD3DX12_SHADER_BYTECODE(shader->_ps.Get()); }
	if (shader->_type & st_shader_type_domain) { pipeline_desc.DS = CD3DX12_SHADER_BYTECODE(shader->_ds.Get()); }
	if (shader->_type & st_shader_type_hull) { pipeline_desc.HS = CD3DX12_SHADER_BYTECODE(shader->_hs.Get()); }
	if (shader->_type & st_shader_type_geometry) { pipeline_desc.GS = CD3DX12_SHADER_BYTECODE(shader->_gs.Get()); }

	pipeline_desc.SampleMask = desc._sample_mask;

	// Rasterizer state.
	D3D12_RASTERIZER_DESC raster_desc = {};
	raster_desc.CullMode = convert_cull_mode(desc._rasterizer_desc._cull_mode);
	raster_desc.FillMode = convert_fill_mode(desc._rasterizer_desc._fill_mode);
	raster_desc.FrontCounterClockwise = !desc._rasterizer_desc._winding_order_clockwise;
	raster_desc.DepthClipEnable = true;

	pipeline_desc.RasterizerState = raster_desc;

	// Blend state.
	D3D12_BLEND_DESC blend_desc = {};
	blend_desc.AlphaToCoverageEnable = desc._blend_desc._alpha_to_coverage;
	blend_desc.IndependentBlendEnable = desc._blend_desc._independent_blend;
	for (uint32_t target_itr = 0; target_itr < desc._render_target_count; ++target_itr)
	{
		D3D12_RENDER_TARGET_BLEND_DESC target_blend = {};
		target_blend.BlendEnable = desc._blend_desc._target_blend[target_itr]._blend;
		target_blend.BlendOp = convert_blend_op(desc._blend_desc._target_blend[target_itr]._blend_op);
		target_blend.BlendOpAlpha = convert_blend_op(desc._blend_desc._target_blend[target_itr]._blend_op_alpha);
		target_blend.DestBlend = convert_blend(desc._blend_desc._target_blend[target_itr]._dst_blend);
		target_blend.DestBlendAlpha = convert_blend(desc._blend_desc._target_blend[target_itr]._dst_blend_alpha);
		target_blend.LogicOp = convert_logic_op(desc._blend_desc._target_blend[target_itr]._logic_op);
		target_blend.LogicOpEnable = desc._blend_desc._target_blend[target_itr]._logic;
		target_blend.RenderTargetWriteMask = desc._blend_desc._target_blend[target_itr]._write_mask;
		target_blend.SrcBlend = convert_blend(desc._blend_desc._target_blend[target_itr]._src_blend);
		target_blend.SrcBlendAlpha = convert_blend(desc._blend_desc._target_blend[target_itr]._src_blend_alpha);

		blend_desc.RenderTarget[target_itr] = target_blend;
	}

	pipeline_desc.BlendState = blend_desc;

	// Depth/stencil state.
	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = desc._depth_stencil_desc._depth_enable;
	depth_stencil_desc.DepthWriteMask = convert_depth_write_mask(desc._depth_stencil_desc._depth_mask);
	depth_stencil_desc.DepthFunc = convert_comparison_func(desc._depth_stencil_desc._depth_compare);
	depth_stencil_desc.StencilEnable = desc._depth_stencil_desc._stencil_enable;
	depth_stencil_desc.StencilReadMask = desc._depth_stencil_desc._stencil_read_mask;
	depth_stencil_desc.StencilWriteMask = desc._depth_stencil_desc._stencil_write_mask;

	D3D12_DEPTH_STENCILOP_DESC front_desc = {};
	front_desc.StencilFailOp = convert_stencil_op(desc._depth_stencil_desc._front_stencil._stencil_fail_op);
	front_desc.StencilDepthFailOp = convert_stencil_op(desc._depth_stencil_desc._front_stencil._depth_fail_op);
	front_desc.StencilPassOp = convert_stencil_op(desc._depth_stencil_desc._front_stencil._stencil_pass_op);
	front_desc.StencilFunc = convert_comparison_func(desc._depth_stencil_desc._front_stencil._stencil_func);
	depth_stencil_desc.FrontFace = front_desc;

	D3D12_DEPTH_STENCILOP_DESC back_desc = {};
	back_desc.StencilFailOp = convert_stencil_op(desc._depth_stencil_desc._back_stencil._stencil_fail_op);
	back_desc.StencilDepthFailOp = convert_stencil_op(desc._depth_stencil_desc._back_stencil._depth_fail_op);
	back_desc.StencilPassOp = convert_stencil_op(desc._depth_stencil_desc._back_stencil._stencil_pass_op);
	back_desc.StencilFunc = convert_comparison_func(desc._depth_stencil_desc._back_stencil._stencil_func);
	depth_stencil_desc.BackFace = back_desc;

	pipeline_desc.DepthStencilState = depth_stencil_desc;

	// Primitive topology.
	pipeline_desc.PrimitiveTopologyType = convert_topology_type(desc._primitive_topology_type);

	// Render targets.
	pipeline_desc.NumRenderTargets = desc._render_target_count;
	for (uint32_t target_itr = 0; target_itr < desc._render_target_count; ++target_itr)
	{
		pipeline_desc.RTVFormats[target_itr] = convert_format(desc._render_target_formats[target_itr]);
	}

	// Depth/stencil format.
	pipeline_desc.DSVFormat = convert_format(desc._depth_stencil_format);

	// Multisampling.
	pipeline_desc.SampleDesc.Count = desc._sample_desc._count;
	pipeline_desc.SampleDesc.Quality = desc._sample_desc._quality;

	// Create the pipeline state object.
	_device->CreateGraphicsPipelineState(
		&pipeline_desc,
		__uuidof(ID3D12PipelineState),
		(void**)pipeline->_pipeline.GetAddressOf());

	return std::move(pipeline);
}

std::unique_ptr<st_vertex_format> st_dx12_graphics_context::create_vertex_format(
	const st_vertex_attribute* attributes,
	uint32_t attribute_count)
{
	std::unique_ptr<st_dx12_vertex_format> vertex_format = std::make_unique<st_dx12_vertex_format>();

	// TODO: Group this into common code.
	size_t vertex_size = 0;

	for (uint32_t itr = 0; itr < attribute_count; ++itr)
	{
		const st_vertex_attribute* attr = &attributes[itr];

		vertex_size += bits_per_pixel(attr->_format) / 8;
	}

	vertex_format->_vertex_size = (uint32_t)vertex_size;

	// Create the element descriptions.
	size_t offset = 0;
	for (uint32_t itr = 0; itr < attribute_count; ++itr)
	{
		const st_vertex_attribute* attr = &attributes[itr];

		LPCSTR semantic_name;
		UINT semantic_index = 0;
		DXGI_FORMAT format = convert_format(attr->_format);
		UINT input_slot;
		UINT aligned_byte_offset = 0;
		D3D12_INPUT_CLASSIFICATION classification = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

		switch (attr->_type)
		{
		case st_vertex_attribute_position:
			semantic_name = "POSITION";
			break;
		case st_vertex_attribute_normal:
			semantic_name = "NORMAL";
			break;
		case st_vertex_attribute_binormal:
			semantic_name = "BINORMAL";
			break;
		case st_vertex_attribute_tangent:
			semantic_name = "TANGENT";
			break;
		case st_vertex_attribute_joints:
			semantic_name = "JOINTS";
			break;
		case st_vertex_attribute_color:
			semantic_name = "COLOR";
			break;
		case st_vertex_attribute_weights:
			semantic_name = "WEIGHT";
			break;
		case st_vertex_attribute_uv:
			semantic_name = "UV";
			break;
		default:
			assert(false);
			break;
		}

		vertex_format->_elements.push_back({
			semantic_name,
			0,
			format,
			0,
			(UINT)offset,
			classification,
			0
			});

		offset += bytes_per_pixel(attr->_format);
	}

	vertex_format->_input_layout.NumElements = attribute_count;
	vertex_format->_input_layout.pInputElementDescs = &vertex_format->_elements[0];

	return std::move(vertex_format);
}

std::unique_ptr<st_render_pass> st_dx12_graphics_context::create_render_pass(
	uint32_t count,
	st_target_desc* targets,
	st_target_desc* depth_stencil)
{
	std::unique_ptr<st_dx12_render_pass> render_pass = std::make_unique<st_dx12_render_pass>();

	// Naively, create the viewport from the first target.
	if (count > 0)
	{
		const st_dx12_texture* t = static_cast<const st_dx12_texture*>(targets[0]._target->get_texture());

		render_pass->_viewport =
		{
			0,
			0,
			float(t->_width),
			float(t->_height),
			0.0f,
			1.0f,
		};
	}
	else if (depth_stencil)
	{
		render_pass->_viewport =
		{
			0,
			0,
			float(depth_stencil->_target->get_width()),
			float(depth_stencil->_target->get_height()),
			0.0f,
			1.0f,
		};
	}
	render_pass->_framebuffer = std::make_unique<st_dx12_framebuffer>(
		count,
		targets,
		depth_stencil);

	return std::move(render_pass);
}

void st_dx12_graphics_context::begin_render_pass(
	st_render_pass* _pass,
	const st_clear_value* clear_values,
	const uint8_t clear_count)
{
	st_dx12_render_pass* pass = static_cast<st_dx12_render_pass*>(_pass);

	set_viewport(pass->_viewport);
	pass->_framebuffer->bind(this);
}

void st_dx12_graphics_context::end_render_pass(st_render_pass* pass)
{
}

void st_dx12_graphics_context::begin_loading()
{
	_command_list->Reset(_command_allocators[_frame_index].Get(), nullptr);
}

void st_dx12_graphics_context::end_loading()
{
	// Close the command list and execute it to begin intial GPU setup.
	_command_list->Close();
	ID3D12CommandList* command_lists[] = { _command_list.Get() };
	_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

	// Wait for the intial GPU state to be setup.
	const uint64_t fence_wait = _fence_value;
	_command_queue->Signal(_fence.Get(), fence_wait);
	_fence_value++;

	_fence->SetEventOnCompletion(fence_wait, _fence_event);
	WaitForSingleObject(_fence_event, INFINITE);
}

void st_dx12_graphics_context::begin_frame()
{
	_command_allocators[_frame_index]->Reset();
	_command_list->Reset(_command_allocators[_frame_index].Get(), nullptr);
	_command_list->SetGraphicsRootSignature(_root_signature.Get());

	// Empty all allocations from this heap. Commands using them have finished executing.
	_cbv_srv_heap[_frame_index]->empty();
	ID3D12DescriptorHeap* heaps[] = { _cbv_srv_heap[_frame_index]->get(), _sampler_heap->get() };
	_command_list->SetDescriptorHeaps(_countof(heaps), heaps);

	_dynamic_vertex_bytes_written = 0;
	_dynamic_index_bytes_written = 0;

	_upload_buffer_offset = 0;
}

void st_dx12_graphics_context::end_frame()
{
	_command_list->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_backbuffers[_frame_index].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_COPY_DEST));

	st_dx12_texture* target = static_cast<st_dx12_texture*>(_present_target->get_texture());
	_command_list->CopyResource(_backbuffers[_frame_index].Get(), target->_handle.Get());

	_command_list->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_backbuffers[_frame_index].Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PRESENT));

	HRESULT result = _command_list->Close();
	if (result != S_OK)
	{
		assert(false);
	}
}

void st_dx12_graphics_context::swap()
{
	ID3D12CommandList* command_lists[] = { _command_list.Get() };
	_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

	_swap_chain->Present(1, 0);

	// TODO: Better parallelization.
	const uint64_t fence = _fence_value;
	_command_queue->Signal(_fence.Get(), fence);
	_fence_value++;

	if (_fence->GetCompletedValue() < fence)
	{
		_fence->SetEventOnCompletion(fence, _fence_event);
		WaitForSingleObject(_fence_event, INFINITE);
	}

	_frame_index = _swap_chain->GetCurrentBackBufferIndex();
}

void st_dx12_graphics_context::begin_marker(const std::string& marker)
{
	PIXBeginEvent(_command_list.Get(), 0, marker.c_str());
}

void st_dx12_graphics_context::end_marker()
{
	PIXEndEvent(_command_list.Get());
}

void st_dx12_graphics_context::destroy_target(st_dx12_descriptor target)
{
	_rtv_heap->deallocate_handle(target);
}

st_dx12_descriptor st_dx12_graphics_context::create_constant_buffer_view(
	D3D12_GPU_VIRTUAL_ADDRESS gpu_address,
	size_t size)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
	cbv_desc.BufferLocation = gpu_address;
	cbv_desc.SizeInBytes = align_value(size, 256);

	st_dx12_cpu_descriptor_handle cbv_handle = _resource_heap->allocate_handle();
	_device->CreateConstantBufferView(&cbv_desc, cbv_handle._handle);

	return cbv_handle._offset;
}

void st_dx12_graphics_context::destroy_constant_buffer_view(st_dx12_descriptor offset)
{
	_resource_heap->deallocate_handle(offset);
}

st_dx12_descriptor st_dx12_graphics_context::create_shader_resource_view(
	ID3D12Resource* resource,
	e_st_format format,
	uint32_t levels)
{
	// Create the shader resource view.
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = convert_format(format);
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = levels;

	st_dx12_cpu_descriptor_handle srv_handle = _resource_heap->allocate_handle();
	_device->CreateShaderResourceView(resource, &srv_desc, srv_handle._handle);

	return srv_handle._offset;
}

void st_dx12_graphics_context::destroy_shader_resource_view(st_dx12_descriptor offset)
{
	_resource_heap->deallocate_handle(offset);
}

st_dx12_descriptor st_dx12_graphics_context::create_buffer_view(
	ID3D12Resource* resource,
	uint32_t count,
	size_t element_size)
{
	// Create the shader resource view.
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Buffer.NumElements = count;
	srv_desc.Buffer.StructureByteStride = element_size;
	srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	st_dx12_cpu_descriptor_handle srv_handle = _resource_heap->allocate_handle();
	_device->CreateShaderResourceView(resource, &srv_desc, srv_handle._handle);

	return srv_handle._offset;
}

void st_dx12_graphics_context::destroy_buffer_view(st_dx12_descriptor offset)
{
	_resource_heap->deallocate_handle(offset);
}

#endif
