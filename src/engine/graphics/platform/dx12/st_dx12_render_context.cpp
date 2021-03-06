/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_render_context.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <core/st_core.h>

#include <graphics/platform/dx12/st_dx12_pipeline_state.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_texture_loader.h>

#include <system/st_window.h>

#include <cassert>
#include <cstdio>
#include <vector>

st_dx12_render_context* st_dx12_render_context::_this = nullptr;

st_dx12_render_context::st_dx12_render_context(const st_window* window)
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
	swap_chain_desc.BufferCount = k_backbuffer_count;
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
	// TODO: The render target count could either be more dynamic, or defined better.
	_rtv_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		k_backbuffer_count + 16,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	_dsv_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		16,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	_cbv_srv_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		k_max_shader_resources,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	_sampler_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		k_max_samplers,
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	// Create the descriptor heap for imgui.
	_gui_srv_heap = std::make_unique<st_dx12_descriptor_heap>(
		_device.Get(),
		1,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	// Create the data upload heap. Let's just make it 128K for now.
	create_buffer(128 * 1024 * 1024, _upload_buffer.GetAddressOf());
	
	// Map the upload buffer.
	D3D12_RANGE map_range{0, 0};
	_upload_buffer->Map(0, &map_range, reinterpret_cast<void**>(&_upload_buffer_head));

	// Create frame resources.
	for (uint32_t rtv_itr = 0; rtv_itr < k_backbuffer_count; ++rtv_itr)
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

	// Create the depth/stencil buffer.
	D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil_desc{};
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_desc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depth_opt_clear_value{};
	depth_opt_clear_value.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_opt_clear_value.DepthStencil.Depth = 1.0f;
	depth_opt_clear_value.DepthStencil.Stencil = 0;

	// TODO: Replace this with a more robust create_buffer.
	result = _device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			window->get_width(),
			window->get_height(),
			1,
			0,
			1,
			0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depth_opt_clear_value,
		__uuidof(ID3D12Resource),
		(void**)&_depth_stencil);

	if (result != S_OK)
	{
		assert(false);
	}

	ST_NAME_DX12_OBJECT(_depth_stencil, str_to_wstr("Depth-Stencil").c_str());

	st_dx12_cpu_descriptor_handle dsv_handle = _dsv_heap->allocate_handle();
	_device->CreateDepthStencilView(_depth_stencil.Get(), &depth_stencil_desc, dsv_handle._handle);

	for (uint32_t ca_itr = 0; ca_itr < k_backbuffer_count; ++ca_itr)
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
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 4, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0);

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
	create_buffer(k_dynamic_buffer_size, _dynamic_vertex_buffer.GetAddressOf());
	create_buffer(k_dynamic_buffer_size, _dynamic_index_buffer.GetAddressOf());

	// Set the global instance.
	_this = this;

	begin_frame();

	// Create the faux backbuffer target.
	_present_target = std::make_unique<st_render_texture>(
		window->get_width(),
		window->get_height(),
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::color_target | e_st_texture_usage::copy_source,
		st_texture_state_copy_source,
		st_vec4f{ 0.0f, 0.0f, 0.0f, 1.0f });
	_present_target->set_name("Present Target");
}

st_dx12_render_context::~st_dx12_render_context()
{
	_present_target = nullptr;
}

void st_dx12_render_context::acquire()
{
}

void st_dx12_render_context::release()
{
}

void st_dx12_render_context::set_pipeline_state(const st_dx12_pipeline_state* state)
{
	_command_list->SetPipelineState(state->get_state());
}

void st_dx12_render_context::set_viewport(const D3D12_VIEWPORT& viewport)
{
	_command_list->RSSetViewports(1, &viewport);
}

void st_dx12_render_context::set_scissor(int left, int top, int right, int bottom)
{
	D3D12_RECT rect;
	rect.left = left;
	rect.right = right;
	rect.top = top;
	rect.bottom = bottom;

	_command_list->RSSetScissorRects(1, &rect);
}

void st_dx12_render_context::set_clear_color(float r, float g, float b, float a)
{
	_clear_color[0] = r;
	_clear_color[1] = g;
	_clear_color[2] = b;
	_clear_color[3] = a;
}

void st_dx12_render_context::set_shader_resource_table(uint32_t offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE srv_handle = _cbv_srv_heap->get_handle_gpu(offset);
	_command_list->SetGraphicsRootDescriptorTable(0, srv_handle);
}

void st_dx12_render_context::set_sampler_table(uint32_t offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE sampler_handle = _sampler_heap->get_handle_gpu(offset);
	_command_list->SetGraphicsRootDescriptorTable(1, sampler_handle);
}

void st_dx12_render_context::set_constant_buffer_table(uint32_t offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE cbv_handle = _cbv_srv_heap->get_handle_gpu(offset);
	_command_list->SetGraphicsRootDescriptorTable(2, cbv_handle);
}

void st_dx12_render_context::set_buffer_table(uint32_t offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE buffer_handle = _cbv_srv_heap->get_handle_gpu(offset);
	_command_list->SetGraphicsRootDescriptorTable(3, buffer_handle);
}

void st_dx12_render_context::set_render_targets(
	uint32_t count,
	class st_render_texture** targets,
	class st_render_texture* depth_stencil)
{
	for (uint32_t target_itr = 0; target_itr < count; ++target_itr)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE target_handle = _rtv_heap->get_handle_cpu(targets[target_itr]->get_rtv());
		_bound_targets[target_itr] = target_handle;
	}

	// Clear out stale render targets.
	for (uint32_t target_itr = count; target_itr < 8; ++target_itr)
	{
		_bound_targets[target_itr] = {};
	}

	if (depth_stencil)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE depth_handle = _dsv_heap->get_handle_cpu(depth_stencil->get_rtv());
		_bound_depth_stencil = depth_handle;
	}

	_command_list->OMSetRenderTargets(
		count,
		_bound_targets,
		false,
		depth_stencil ? &_bound_depth_stencil : nullptr);
}

void st_dx12_render_context::clear(unsigned int clear_flags)
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

void st_dx12_render_context::draw(const st_static_drawcall& drawcall)
{
	_command_list->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY)drawcall._draw_mode);
	_command_list->IASetVertexBuffers(0, 1, drawcall._vertex_buffer_view);
	_command_list->IASetIndexBuffer(drawcall._index_buffer_view);
	_command_list->DrawIndexedInstanced(drawcall._index_count, 1, 0, 0, 0);
}

void st_dx12_render_context::draw(const st_dynamic_drawcall& drawcall)
{
	// TODO: Dynamic buffer limit checking.

	uint8_t* buffer_begin;
	D3D12_RANGE range = { 0, 0 };
	HRESULT result = _dynamic_vertex_buffer->Map(0, &range, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_vertex_bytes_written;

	D3D12_VERTEX_BUFFER_VIEW dynamic_vertex_buffer_view;
	dynamic_vertex_buffer_view.BufferLocation = _dynamic_vertex_buffer->GetGPUVirtualAddress() + _dynamic_vertex_bytes_written;
	dynamic_vertex_buffer_view.StrideInBytes = 20;
	dynamic_vertex_buffer_view.SizeInBytes = drawcall._positions.size() * 20;

	if (result != S_OK)
	{
		assert(false);
	}

	struct st_dynamic_vertex
	{
		st_vec3f _pos;
		st_vec2f _uv;
	};

	std::vector<st_dynamic_vertex> verts;

	for (uint32_t vert_itr = 0; vert_itr < drawcall._positions.size(); ++vert_itr)
	{
		verts.push_back({ drawcall._positions[vert_itr], st_vec2f() });
	}

	for (uint32_t vert_itr = 0; vert_itr < drawcall._texcoords.size(); ++vert_itr)
	{
		verts[vert_itr]._uv = drawcall._texcoords[vert_itr];
	}

	memcpy(buffer_begin, &verts[0], sizeof(st_dynamic_vertex) * verts.size());
	_dynamic_vertex_buffer->Unmap(0, nullptr);
	_dynamic_vertex_bytes_written += sizeof(st_dynamic_vertex) * verts.size();

	range.Begin = _dynamic_index_bytes_written;
	result = _dynamic_index_buffer->Map(0, &range, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_index_bytes_written;

	D3D12_INDEX_BUFFER_VIEW dynamic_index_buffer_view;
	dynamic_index_buffer_view.BufferLocation = _dynamic_index_buffer->GetGPUVirtualAddress() + _dynamic_index_bytes_written;
	dynamic_index_buffer_view.Format = DXGI_FORMAT_R16_UINT;
	dynamic_index_buffer_view.SizeInBytes = sizeof(uint16_t) * drawcall._indices.size();

	if (result != S_OK)
	{
		assert(false);
	}

	memcpy(buffer_begin, &drawcall._indices[0], sizeof(uint16_t) * drawcall._indices.size());
	_dynamic_index_buffer->Unmap(0, nullptr);
	_dynamic_index_bytes_written += sizeof(uint16_t) * drawcall._indices.size();

	st_static_drawcall static_draw;
	static_draw._draw_mode = drawcall._draw_mode;
	static_draw._vertex_buffer_view = &dynamic_vertex_buffer_view;
	static_draw._index_buffer_view = &dynamic_index_buffer_view;
	static_draw._index_count = drawcall._indices.size();

	draw(static_draw);
}

void st_dx12_render_context::transition_backbuffer_to_target()
{
	_present_target->transition(this, st_texture_state_render_target);
}

void st_dx12_render_context::transition_backbuffer_to_present()
{
	_present_target->transition(this, st_texture_state_copy_source);
}

void st_dx12_render_context::transition(
	st_dx12_texture* texture,
	e_st_texture_state old_state,
	e_st_texture_state new_state)
{
	assert(old_state != new_state);

	// TODO: It's bad practice to transition one at a time. These should be accumulated
	// and then flushed all at once.
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		texture->get_resource(),
		D3D12_RESOURCE_STATES(old_state),
		D3D12_RESOURCE_STATES(new_state));

	_command_list->ResourceBarrier(1, &barrier);
}

void st_dx12_render_context::begin_loading()
{
	_command_list->Reset(_command_allocators[_frame_index].Get(), nullptr);
}

void st_dx12_render_context::end_loading()
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

void st_dx12_render_context::begin_frame()
{
	_command_allocators[_frame_index]->Reset();
	_command_list->Reset(_command_allocators[_frame_index].Get(), nullptr);
	_command_list->SetGraphicsRootSignature(_root_signature.Get());

	ID3D12DescriptorHeap* heaps[] = { _cbv_srv_heap->get(), _sampler_heap->get() };
	_command_list->SetDescriptorHeaps(_countof(heaps), heaps);

	_dynamic_vertex_bytes_written = 0;
	_dynamic_index_bytes_written = 0;

	_upload_buffer_offset = 0;
}

void st_dx12_render_context::end_frame()
{
	_command_list->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_backbuffers[_frame_index].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_COPY_DEST));

	_command_list->CopyResource(_backbuffers[_frame_index].Get(), _present_target->get_resource());

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

void st_dx12_render_context::swap()
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

void st_dx12_render_context::begin_marker(const std::string& marker)
{
	PIXBeginEvent(_command_list.Get(), 0, marker.c_str());
}

void st_dx12_render_context::end_marker()
{
	PIXEndEvent(_command_list.Get());
}

void st_dx12_render_context::create_graphics_pipeline_state(
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline_desc,
	ID3D12PipelineState** pipeline_state)
{
	_device->CreateGraphicsPipelineState(
		&pipeline_desc,
		__uuidof(ID3D12PipelineState),
		(void**)pipeline_state);
}

void st_dx12_render_context::create_buffer(size_t size, ID3D12Resource** resource)
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

void st_dx12_render_context::create_texture(
	uint32_t width,
	uint32_t height,
	uint32_t mip_count,
	e_st_format format,
	e_st_texture_usage_flags usage,
	e_st_texture_state initial_state,
	void* data,
	ID3D12Resource** resource)
{
	DXGI_FORMAT real_format = (DXGI_FORMAT)format;

	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
	if (usage & e_st_texture_usage::color_target) flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (usage & e_st_texture_usage::depth_target) flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	if (usage & e_st_texture_usage::storage) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_RESOURCE_DESC texture_desc{};
	texture_desc.MipLevels = mip_count;
	texture_desc.Format = real_format;
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.Flags = flags;
	texture_desc.DepthOrArraySize = 1;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

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
		nullptr,
		__uuidof(ID3D12Resource),
		(void**)resource);

	if (result != S_OK)
	{
		assert(false);
	}

	if (data)
	{
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		uint8_t* bits = reinterpret_cast<uint8_t*>(data);
		for (uint32_t level = 0; level < mip_count; ++level)
		{
			uint32_t level_width = width >> level;
			uint32_t level_height = height >> level;

			size_t row_bytes;
			size_t num_bytes;
			get_surface_info(
				level_width,
				level_height,
				format,
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

		size_t alloc_size = (sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(uint32_t) + sizeof(uint64_t)) * mip_count;
		auto layouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(malloc(alloc_size));

		uint64_t* row_sizes_bytes = reinterpret_cast<uint64_t*>(layouts + mip_count);
		uint32_t* row_count = reinterpret_cast<uint32_t*>(row_sizes_bytes + mip_count);
		uint64_t required_size = 0;
		_device->GetCopyableFootprints(&texture_desc, 0, mip_count, 0, layouts, row_count, row_sizes_bytes, &required_size);

		_upload_buffer_offset = align_value(_upload_buffer_offset, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
		size_t initial_offset = _upload_buffer_offset;

		for (uint32_t i = 0; i < mip_count; ++i)
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

		for (uint32_t i = 0; i < mip_count; ++i)
		{
			// Copy the upload heap to the texture 2D.
			D3D12_TEXTURE_COPY_LOCATION dest_location
			{
				*resource,
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
			*resource,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATES(initial_state)));
}

void st_dx12_render_context::create_target(
	uint32_t width,
	uint32_t height,
	e_st_format format,
	const st_vec4f& clear,
	e_st_texture_state initial_state,
	ID3D12Resource** resource,
	st_dx12_descriptor* rtv_offset)
{
	D3D12_RESOURCE_FLAGS flags;

	switch (format)
	{
	case st_format_d24_unorm_s8_uint:
		flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		break;
	default:
		flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		break;
	}

	D3D12_RESOURCE_DESC target_desc = {};
	target_desc.Width = width;
	target_desc.Height = height;
	target_desc.MipLevels = 1;
	target_desc.Format = (DXGI_FORMAT)format;
	target_desc.Flags = flags;
	target_desc.DepthOrArraySize = 1;
	target_desc.SampleDesc.Count = 1;
	target_desc.SampleDesc.Quality = 0;
	target_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE clear_value = {};
	clear_value.Format = (DXGI_FORMAT)format;

	if (format == st_format_d24_unorm_s8_uint)
	{
		clear_value.DepthStencil.Depth = clear.x;
		clear_value.DepthStencil.Stencil = (uint8_t)clear.y;
	}
	else
	{
		clear_value.Color[0] = clear.x;
		clear_value.Color[1] = clear.y;
		clear_value.Color[2] = clear.z;
		clear_value.Color[3] = clear.w;
	}

	D3D12_HEAP_PROPERTIES heap_properties{
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		1,
		1
	};

	HRESULT hr = _device->CreateCommittedResource(
		&heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&target_desc,
		D3D12_RESOURCE_STATES(initial_state),
		&clear_value,
		__uuidof(ID3D12Resource),
		(void**)resource);

	if (format == st_format_d24_unorm_s8_uint)
	{
		ST_NAME_DX12_OBJECT(*resource, str_to_wstr("Depth-Stencil Target").c_str());

		// Create the depth/stencil view.
		st_dx12_cpu_descriptor_handle dsv_handle = _dsv_heap->allocate_handle();
		_device->CreateDepthStencilView(*resource, nullptr, dsv_handle._handle);

		*rtv_offset = dsv_handle._offset;
	}
	else
	{
		ST_NAME_DX12_OBJECT(*resource, str_to_wstr("Render Target").c_str());

		// Create the render target view.
		st_dx12_cpu_descriptor_handle rtv_handle = _rtv_heap->allocate_handle();
		_device->CreateRenderTargetView(*resource, nullptr, rtv_handle._handle);

		*rtv_offset = rtv_handle._offset;
	}
}

void st_dx12_render_context::destroy_target(st_dx12_descriptor target)
{
	_rtv_heap->deallocate_handle(target);
}

st_dx12_descriptor st_dx12_render_context::create_constant_buffer_view(
	D3D12_GPU_VIRTUAL_ADDRESS gpu_address,
	size_t size)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
	cbv_desc.BufferLocation = gpu_address;
	cbv_desc.SizeInBytes = align_value(size, 256);

	st_dx12_cpu_descriptor_handle cbv_handle = _cbv_srv_heap->allocate_handle();
	_device->CreateConstantBufferView(&cbv_desc, cbv_handle._handle);

	return cbv_handle._offset;
}

void st_dx12_render_context::destroy_constant_buffer_view(st_dx12_descriptor offset)
{
	_cbv_srv_heap->deallocate_handle(offset);
}

st_dx12_descriptor st_dx12_render_context::create_shader_resource_view(
	ID3D12Resource* resource,
	e_st_format format,
	uint32_t levels)
{
	// Create the shader resource view.
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = (DXGI_FORMAT)format;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = levels;

	st_dx12_cpu_descriptor_handle srv_handle = _cbv_srv_heap->allocate_handle();
	_device->CreateShaderResourceView(resource, &srv_desc, srv_handle._handle);

	return srv_handle._offset;
}

void st_dx12_render_context::destroy_shader_resource_view(st_dx12_descriptor offset)
{
	_cbv_srv_heap->deallocate_handle(offset);
}

st_dx12_descriptor st_dx12_render_context::create_shader_sampler()
{
	// Create the sampler.
	D3D12_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MaxAnisotropy = 1;
	sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	st_dx12_cpu_descriptor_handle sampler_handle = _sampler_heap->allocate_handle();
	_device->CreateSampler(&sampler_desc, sampler_handle._handle);

	return sampler_handle._offset;
}

void st_dx12_render_context::destroy_shader_sampler(st_dx12_descriptor offset)
{
	_sampler_heap->deallocate_handle(offset);
}

st_dx12_descriptor st_dx12_render_context::create_buffer_view(
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

	st_dx12_cpu_descriptor_handle srv_handle = _cbv_srv_heap->allocate_handle();
	_device->CreateShaderResourceView(resource, &srv_desc, srv_handle._handle);

	return srv_handle._offset;
}

void st_dx12_render_context::destroy_buffer_view(st_dx12_descriptor offset)
{
	_cbv_srv_heap->deallocate_handle(offset);
}

st_dx12_render_context* st_dx12_render_context::get()
{
	return _this;
}

#endif
