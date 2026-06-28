#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>

#include <memory>

class st_dx12_command_list : public st_command_list
{
public:

	st_dx12_command_list(ID3D12GraphicsCommandList* command_list, class st_dx12_device* device);
	~st_dx12_command_list();

	void begin(class st_command_allocator* command_allocator) override;
	void end() override;

	void set_pipeline(const st_pipeline* state) override;
	void set_compute_pipeline(const st_pipeline* state) override;
	void set_viewport(const st_viewport& viewport) override;
	void set_scissor(int left, int top, int right, int bottom) override;
	void set_blend_factor(float r, float g, float b, float a) override;

	void draw(const struct st_static_drawcall& drawcall) override;
	void draw(const struct st_dynamic_drawcall& drawcall) override;

	// Compute.
	void dispatch(const st_dispatch_args& args) override;

	// Debug.
	void begin_marker(const std::string& marker) override;
	void end_marker() override;

	// Textures.
	void transition(st_texture* texture, e_st_texture_state new_state) override;

	// Buffers.
	void update_buffer(st_buffer* buffer, void* data, const uint32_t offset, const uint32_t count) override;

	// Resource tables.
	void bind_resources(st_resource_table* table) override;
	void bind_compute_resources(st_resource_table* table) override;

	// Render passes.
	void begin_render_pass(
		st_render_pass* pass,
		st_framebuffer* framebuffer,
		const st_clear_value* clear_values,
		const uint8_t clear_count) override;
	void end_render_pass(st_render_pass* pass, st_framebuffer* framebuffer) override;

	ID3D12CommandList* get() { return _d3d_command_list.Get(); }

private:

	void set_render_targets(
		uint32_t count,
		st_texture_view** targets,
		st_texture_view* depth_stencil);

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _d3d_command_list;

	// Keep a pointer to the device to use for descriptor management.
	class st_dx12_device* _device;

	// Heap used for drawing. Descriptors are copied to blocks in this heap with each draw.
	std::unique_ptr<st_dx12_descriptor_heap> _cbv_srv_heap[k_max_frames];
	std::unique_ptr<st_dx12_descriptor_heap> _sampler_heap[k_max_frames];

	// Dynamic geometry buffers.
	std::unique_ptr<st_buffer> _dynamic_vertex_buffer;
	size_t _dynamic_vertex_bytes_written = 0;
	std::unique_ptr<st_buffer> _dynamic_index_buffer;
	size_t _dynamic_index_bytes_written = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE _bound_depth_stencil;
	D3D12_CPU_DESCRIPTOR_HANDLE _bound_targets[8] = {};

	uint32_t _frame_index = 0;
};

#endif
