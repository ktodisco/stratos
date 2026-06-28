/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_command_list.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/st_drawcall.h>

#include <graphics/platform/dx12/st_dx12_command_allocator.h>
#include <graphics/platform/dx12/st_dx12_conversion.h>
#include <graphics/platform/dx12/st_dx12_device.h>

#include <cassert>
#include <vector>

st_dx12_command_list::st_dx12_command_list(ID3D12GraphicsCommandList* command_list, st_dx12_device* device)
	: _d3d_command_list(command_list), _device(device)
{
	for (uint32_t h_itr = 0; h_itr < k_max_frames; ++h_itr)
	{
		_cbv_srv_heap[h_itr] = std::make_unique<st_dx12_descriptor_heap>(
			_device->get(),
			k_max_shader_resources,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		_sampler_heap[h_itr] = std::make_unique<st_dx12_descriptor_heap>(
			_device->get(),
			k_max_samplers,
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	// Set up the dynamic geometry buffers.
	// TODO: Better way of managing this constant.
	const size_t k_dynamic_buffer_size = 16 * 1000 * 1000;

	st_buffer_desc vertex_buffer_desc;
	vertex_buffer_desc._element_size = sizeof(uint8_t);
	vertex_buffer_desc._count = k_dynamic_buffer_size;
	vertex_buffer_desc._usage = e_st_buffer_usage::vertex | e_st_buffer_usage::transfer_dest;
	_dynamic_vertex_buffer = device->create_buffer(vertex_buffer_desc);

	st_buffer_desc index_buffer_desc;
	index_buffer_desc._element_size = sizeof(uint8_t);
	index_buffer_desc._count = k_dynamic_buffer_size;
	index_buffer_desc._usage = e_st_buffer_usage::index | e_st_buffer_usage::transfer_dest;
	_dynamic_index_buffer = device->create_buffer(index_buffer_desc);
}

st_dx12_command_list::~st_dx12_command_list()
{
	_d3d_command_list = nullptr;
}

void st_dx12_command_list::begin(st_command_allocator* command_allocator_)
{
	st_dx12_command_allocator* command_allocator = static_cast<st_dx12_command_allocator*>(command_allocator_);

	_d3d_command_list->Reset(command_allocator->get(), nullptr);

	_frame_index = (_frame_index + 1) % k_max_frames;
}

void st_dx12_command_list::end()
{
	_d3d_command_list->Close();
}

void st_dx12_command_list::set_pipeline(const st_pipeline* state_)
{
	const st_dx12_pipeline* state = static_cast<const st_dx12_pipeline*>(state_);
	_d3d_command_list->SetPipelineState(state->_pipeline.Get());

	_d3d_command_list->OMSetStencilRef(state->_stencil_ref);
}

void st_dx12_command_list::set_compute_pipeline(const st_pipeline* state_)
{
	const st_dx12_pipeline* state = static_cast<const st_dx12_pipeline*>(state_);
	_d3d_command_list->SetPipelineState(state->_pipeline.Get());
}

void st_dx12_command_list::set_viewport(const st_viewport& viewport)
{
	D3D12_VIEWPORT v;
	v.TopLeftX = viewport._x;
	v.TopLeftY = viewport._y;
	v.Width = viewport._width;
	v.Height = viewport._height;
	v.MinDepth = viewport._min_depth;
	v.MaxDepth = viewport._max_depth;
	_d3d_command_list->RSSetViewports(1, &v);
}

void st_dx12_command_list::set_scissor(int left, int top, int right, int bottom)
{
	D3D12_RECT rect;
	rect.left = left;
	rect.right = right;
	rect.top = top;
	rect.bottom = bottom;

	_d3d_command_list->RSSetScissorRects(1, &rect);
}

void st_dx12_command_list::set_blend_factor(float r, float g, float b, float a)
{
	const float factor[] = { r, g, b, a };
	_d3d_command_list->OMSetBlendFactor(factor);
}

void st_dx12_command_list::set_render_targets(
	uint32_t count,
	st_texture_view** targets,
	st_texture_view* depth_stencil)
{
	for (uint32_t target_itr = 0; target_itr < count; ++target_itr)
	{
		const st_dx12_texture_view* texture_view = static_cast<const st_dx12_texture_view*>(targets[target_itr]);
		D3D12_CPU_DESCRIPTOR_HANDLE target_handle = _device->get_rtv_heap()->get_handle_cpu(texture_view->_handle);
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
		D3D12_CPU_DESCRIPTOR_HANDLE depth_handle = _device->get_dsv_heap()->get_handle_cpu(ds_view->_handle);
		_bound_depth_stencil = depth_handle;
	}

	_d3d_command_list->OMSetRenderTargets(
		count,
		_bound_targets,
		false,
		depth_stencil ? &_bound_depth_stencil : nullptr);
}

void st_dx12_command_list::draw(const struct st_static_drawcall& drawcall)
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

	_d3d_command_list->IASetPrimitiveTopology(convert_topology(drawcall._draw_mode));
	_d3d_command_list->IASetVertexBuffers(0, 1, &vertex_view);
	_d3d_command_list->IASetIndexBuffer(&index_view);
	_d3d_command_list->DrawIndexedInstanced(drawcall._index_count, 1, drawcall._index_offset, drawcall._vertex_offset, 0);
}

void st_dx12_command_list::draw(const struct st_dynamic_drawcall& drawcall)
{
	// TODO: Transfer dynamic geometry responsibility to the application.

	struct st_procedural_vertex
	{
		st_vec3f _pos;
		st_vec3f _color;
	};

	st_dx12_buffer* dynamic_vertex_buffer = static_cast<st_dx12_buffer*>(_dynamic_vertex_buffer.get());
	st_dx12_buffer* dynamic_index_buffer = static_cast<st_dx12_buffer*>(_dynamic_index_buffer.get());

	// TODO: Dynamic buffer limit checking.

	uint8_t* buffer_begin;
	D3D12_RANGE range = { 0, 0 };
	HRESULT result = dynamic_vertex_buffer->_buffer->Map(0, &range, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_vertex_bytes_written;

	D3D12_VERTEX_BUFFER_VIEW dynamic_vertex_view;
	dynamic_vertex_view.BufferLocation = dynamic_vertex_buffer->_buffer->GetGPUVirtualAddress() + _dynamic_vertex_bytes_written;
	dynamic_vertex_view.StrideInBytes = sizeof(st_procedural_vertex);
	dynamic_vertex_view.SizeInBytes = drawcall._positions.size() * sizeof(st_procedural_vertex);

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
	dynamic_vertex_buffer->_buffer->Unmap(0, nullptr);
	_dynamic_vertex_bytes_written += sizeof(st_procedural_vertex) * verts.size();

	range.Begin = _dynamic_index_bytes_written;
	result = dynamic_index_buffer->_buffer->Map(0, &range, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_index_bytes_written;

	D3D12_INDEX_BUFFER_VIEW dynamic_index_view;
	dynamic_index_view.BufferLocation = dynamic_index_buffer->_buffer->GetGPUVirtualAddress() + _dynamic_index_bytes_written;
	dynamic_index_view.Format = DXGI_FORMAT_R16_UINT;
	dynamic_index_view.SizeInBytes = sizeof(uint16_t) * drawcall._indices.size();

	if (result != S_OK)
	{
		assert(false);
	}

	memcpy(buffer_begin, &drawcall._indices[0], sizeof(uint16_t) * drawcall._indices.size());
	dynamic_index_buffer->_buffer->Unmap(0, nullptr);
	_dynamic_index_bytes_written += sizeof(uint16_t) * drawcall._indices.size();

	_d3d_command_list->IASetPrimitiveTopology(convert_topology(drawcall._draw_mode));
	_d3d_command_list->IASetVertexBuffers(0, 1, &dynamic_vertex_view);
	_d3d_command_list->IASetIndexBuffer(&dynamic_index_view);
	_d3d_command_list->DrawIndexedInstanced(drawcall._indices.size(), 1, 0, 0, 0);
}

void st_dx12_command_list::dispatch(const st_dispatch_args& args)
{
	_d3d_command_list->Dispatch(args.group_count_x, args.group_count_y, args.group_count_z);
}

void st_dx12_command_list::begin_marker(const std::string& marker)
{
	PIXBeginEvent(_d3d_command_list.Get(), 0, marker.c_str());
}

void st_dx12_command_list::end_marker()
{
	PIXEndEvent(_d3d_command_list.Get());
}

void st_dx12_command_list::transition(st_texture* texture_, e_st_texture_state new_state)
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

	_d3d_command_list->ResourceBarrier(1, &barrier);
}

void st_dx12_command_list::update_buffer(st_buffer* buffer_, void* data, const uint32_t offset, const uint32_t count)
{
	st_dx12_buffer* buffer = static_cast<st_dx12_buffer*>(buffer_);

	uint8_t* out_data = nullptr;
	D3D12_RANGE range { 0, buffer->_element_size * count };
	HRESULT result = buffer->_buffer->Map(0, &range, (void**)&out_data);

	if (result != S_OK)
	{
		assert(false);
	}

	memcpy(out_data + offset, data, count * buffer->_element_size);

	buffer->_buffer->Unmap(0, &range);
}

void st_dx12_command_list::bind_resources(st_resource_table* table_)
{
	st_dx12_resource_table* table = static_cast<st_dx12_resource_table*>(table_);

	auto set_descriptor_table = [this](e_st_descriptor_slot slot, st_dx12_descriptor_heap* heap, uint32_t offset)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE cbv_handle = heap->get_handle_gpu(offset);
		_d3d_command_list->SetGraphicsRootDescriptorTable(slot, cbv_handle);
	};

	static uint32_t k_invalid_offset = 0xffffffff;

	uint32_t offset = k_invalid_offset;
	for (uint32_t cbv_itr = 0; cbv_itr < table->_cbvs.size(); ++cbv_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _cbv_srv_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->get()->CopyDescriptorsSimple(
			1,
			handle._handle,
			_device->get_resource_heap()->get_handle_cpu(table->_cbvs[cbv_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (table->_cbvs.size() > 0)
		set_descriptor_table(st_descriptor_slot_constants, _cbv_srv_heap[_frame_index].get(), offset);

	offset = k_invalid_offset;
	for (uint32_t srv_itr = 0; srv_itr < table->_srvs.size(); ++srv_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _cbv_srv_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->get()->CopyDescriptorsSimple(
			1,
			handle._handle,
			_device->get_resource_heap()->get_handle_cpu(table->_srvs[srv_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (table->_srvs.size() > 0)
		set_descriptor_table(st_descriptor_slot_textures, _cbv_srv_heap[_frame_index].get(), offset);

	offset = k_invalid_offset;
	for (uint32_t b_itr = 0; b_itr < table->_buffers.size(); ++b_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _cbv_srv_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->get()->CopyDescriptorsSimple(
			1,
			handle._handle,
			_device->get_resource_heap()->get_handle_cpu(table->_buffers[b_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (table->_buffers.size() > 0)
		set_descriptor_table(st_descriptor_slot_buffers, _cbv_srv_heap[_frame_index].get(), offset);

	offset = k_invalid_offset;
	for (uint32_t s_itr = 0; s_itr < table->_samplers.size(); ++s_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _sampler_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->get()->CopyDescriptorsSimple(
			1,
			handle._handle,
			_device->get_static_sampler_heap()->get_handle_cpu(table->_samplers[s_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	}
	if (table->_samplers.size() > 0)
		set_descriptor_table(st_descriptor_slot_samplers, _sampler_heap[_frame_index].get(), offset);
}

void st_dx12_command_list::bind_compute_resources(st_resource_table* table_)
{
	st_dx12_resource_table* table = static_cast<st_dx12_resource_table*>(table_);

	auto set_descriptor_table = [this](e_st_descriptor_slot slot, st_dx12_descriptor_heap* heap, uint32_t offset)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE cbv_handle = heap->get_handle_gpu(offset);
		_d3d_command_list->SetComputeRootDescriptorTable(slot, cbv_handle);
	};

	static uint32_t k_invalid_offset = 0xffffffff;

	uint32_t offset = k_invalid_offset;
	for (uint32_t cbv_itr = 0; cbv_itr < table->_cbvs.size(); ++cbv_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _cbv_srv_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->get()->CopyDescriptorsSimple(
			1,
			handle._handle,
			_device->get_resource_heap()->get_handle_cpu(table->_cbvs[cbv_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (table->_cbvs.size() > 0)
		set_descriptor_table(st_descriptor_slot_constants, _cbv_srv_heap[_frame_index].get(), offset);

	offset = k_invalid_offset;
	for (uint32_t srv_itr = 0; srv_itr < table->_srvs.size(); ++srv_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _cbv_srv_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->get()->CopyDescriptorsSimple(
			1,
			handle._handle,
			_device->get_resource_heap()->get_handle_cpu(table->_srvs[srv_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (table->_srvs.size() > 0)
		set_descriptor_table(st_descriptor_slot_textures, _cbv_srv_heap[_frame_index].get(), offset);

	offset = k_invalid_offset;
	for (uint32_t s_itr = 0; s_itr < table->_samplers.size(); ++s_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _sampler_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->get()->CopyDescriptorsSimple(
			1,
			handle._handle,
			_device->get_static_sampler_heap()->get_handle_cpu(table->_samplers[s_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	}
	if (table->_samplers.size() > 0)
		set_descriptor_table(st_descriptor_slot_samplers, _sampler_heap[_frame_index].get(), offset);

	offset = k_invalid_offset;
	for (uint32_t uav_itr = 0; uav_itr < table->_uavs.size(); ++uav_itr)
	{
		st_dx12_cpu_descriptor_handle handle = _cbv_srv_heap[_frame_index]->allocate_handle();
		if (offset == k_invalid_offset)
			offset = handle._offset;

		_device->get()->CopyDescriptorsSimple(
			1,
			handle._handle,
			_device->get_resource_heap()->get_handle_cpu(table->_uavs[uav_itr]),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	if (table->_uavs.size() > 0)
		set_descriptor_table(st_descriptor_slot_uavs, _cbv_srv_heap[_frame_index].get(), offset);
}

void st_dx12_command_list::begin_render_pass(
	st_render_pass* pass_,
	st_framebuffer* framebuffer_,
	const st_clear_value* clear_values,
	const uint8_t clear_count)
{
	st_dx12_render_pass* pass = static_cast<st_dx12_render_pass*>(pass_);

	set_viewport(pass->_viewport);

	// Bind the framebuffer.
	st_dx12_framebuffer* framebuffer = static_cast<st_dx12_framebuffer*>(framebuffer_);

	for (auto& t : framebuffer->_targets)
	{
		transition(t, st_texture_state_render_target);
	}

	const st_texture_view* ds_view = nullptr;
	if (framebuffer->_depth_stencil)
	{
		transition(framebuffer->_depth_stencil, st_texture_state_depth_target);
	}

	// Bind them.
	set_render_targets(framebuffer->_views.size(), framebuffer->_views.data(), framebuffer->_depth_stencil_view);

	// Perform any clears.
	for (uint32_t att = 0; att < pass->_color_attachments.size(); ++att)
	{
		const st_attachment_desc& attachment = pass->_color_attachments[att];
		if (attachment._load_op == e_st_load_op::clear)
		{
			_d3d_command_list->ClearRenderTargetView(
				_bound_targets[att],
				clear_values[att]._color.axes,
				0,
				nullptr);
		}
	}

	if (framebuffer->_depth_stencil && pass->_depth_attachment._load_op == e_st_load_op::clear)
	{
		_d3d_command_list->ClearDepthStencilView(
			_bound_depth_stencil,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			clear_values[clear_count - 1]._depth_stencil._depth,
			clear_values[clear_count - 1]._depth_stencil._stencil,
			0,
			nullptr);
	}
}

void st_dx12_command_list::end_render_pass(st_render_pass* pass, st_framebuffer* framebuffer)
{
	set_render_targets(0, nullptr, nullptr);
}

#endif
