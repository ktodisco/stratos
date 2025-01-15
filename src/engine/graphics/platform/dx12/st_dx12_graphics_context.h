#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/st_graphics_context.h>

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>

#include <math/st_vec4f.h>

#include <cstdint>
#include <string>

class st_dx12_graphics_context : public st_graphics_context
{
public:

	st_dx12_graphics_context(const class st_window* window);
	~st_dx12_graphics_context();

	void acquire() override;
	void release() override;

	void set_pipeline(const st_pipeline* state) override;
	void set_compute_pipeline(const st_pipeline* state) override;
	void set_viewport(const st_viewport& viewport) override;
	void set_scissor(int left, int top, int right, int bottom) override;
	void set_clear_color(float r, float g, float b, float a) override;
	void set_blend_factor(float r, float g, float b, float a) override;

	void set_render_targets(
		uint32_t count,
		const st_texture_view** targets,
		const st_texture_view* depth_stencil) override;

	void clear(unsigned int clear_flags) override;
	void draw(const struct st_static_drawcall& drawcall) override;
	void draw(const struct st_dynamic_drawcall& drawcall) override;

	// Compute.
	void dispatch(const st_dispatch_args& args) override;

	// Backbuffer.
	st_render_texture* get_present_target() const override;
	// TODO: These are temporary and a generic solution is needed.
	void transition_backbuffer_to_target() override;
	void transition_backbuffer_to_present() override;

	void begin_loading() override;
	void end_loading() override;
	void begin_frame() override;
	void end_frame() override;
	void swap() override;

	void begin_marker(const std::string& marker) override;
	void end_marker() override;

	// Textures.
	// TODO: Fast clear value.
	std::unique_ptr<st_texture> create_texture(const st_texture_desc& desc) override;
	void set_texture_name(st_texture* texture, std::string name) override;
	void transition(
		st_texture* texture,
		e_st_texture_state new_state) override;
	std::unique_ptr<st_texture_view> create_texture_view(st_texture* texture) override;

	// Samplers.
	std::unique_ptr<st_sampler> create_sampler(const st_sampler_desc& desc) override;

	// Buffers.
	std::unique_ptr<st_buffer> create_buffer(const st_buffer_desc& desc) override;
	std::unique_ptr<st_buffer_view> create_buffer_view(st_buffer* buffer) override;
	void map(st_buffer* buffer, uint32_t subresource, const st_range& range, void** outData) override;
	void unmap(st_buffer* buffer, uint32_t subresource, const st_range& range) override;
	void update_buffer(st_buffer* buffer, void* data, const uint32_t offset, const uint32_t count) override;
	void set_buffer_name(st_buffer* buffer, std::string name) override;

	// Constant buffers.
	void add_constant(
		st_buffer* buffer,
		const std::string& name,
		const e_st_shader_constant_type constant_type) override;

	// Resource tables.
	std::unique_ptr<st_resource_table> create_resource_table() override;
	void set_constant_buffers(st_resource_table* table, uint32_t count, st_buffer** cbs) override;
	void set_textures(
		st_resource_table* table,
		uint32_t count,
		st_texture** textures,
		st_sampler** samplers) override;
	void set_buffers(st_resource_table* table, uint32_t count, st_buffer** buffers) override;
	void set_uavs(st_resource_table* table, uint32_t count, st_texture** textures) override;
	void update_textures(st_resource_table* table, uint32_t count, st_texture_view** views) override;
	void bind_resources(st_resource_table* table) override;
	void bind_compute_resources(st_resource_table* table) override;

	// Shaders.
	std::unique_ptr<st_shader> create_shader(const char* filename, uint8_t type) override;

	// Pipelines.
	std::unique_ptr<st_pipeline> create_graphics_pipeline(const struct st_graphics_state_desc& desc) override;
	std::unique_ptr<st_pipeline> create_compute_pipeline(const struct st_compute_state_desc& desc) override;

	// Geometry.
	std::unique_ptr<st_vertex_format> create_vertex_format(
		const st_vertex_attribute* attributes,
		uint32_t attribute_count) override;

	// Render passes.
	std::unique_ptr<st_render_pass> create_render_pass(
		uint32_t count,
		struct st_target_desc* targets,
		struct st_target_desc* depth_stencil) override;
	void begin_render_pass(
		st_render_pass* pass,
		const st_clear_value* clear_values,
		const uint8_t clear_count) override;
	void end_render_pass(st_render_pass* pass) override;

	// Informational.
	e_st_graphics_api get_api() { return e_st_graphics_api::dx12; }

	// API-specific.
	ID3D12Device* get_device() const { return _device.Get(); }
	ID3D12GraphicsCommandList* get_command_list() const { return _command_list.Get(); }
	st_dx12_descriptor_heap* get_gui_heap() const { return _gui_srv_heap.get(); }

private:

	void create_buffer_internal(size_t size, ID3D12Resource** resource);

	// TODO: Get rid of these in favor of the public create_*_view functions.
	st_dx12_descriptor create_constant_buffer_view(
		D3D12_GPU_VIRTUAL_ADDRESS gpu_address,
		size_t size);
	void destroy_constant_buffer_view(st_dx12_descriptor offset);
	st_dx12_descriptor create_shader_resource_view(
		ID3D12Resource* resource,
		e_st_format format,
		uint32_t levels);
	void destroy_shader_resource_view(st_dx12_descriptor offset);
	// TODO: Combine this with create_shader_resource_view.
	st_dx12_descriptor create_buffer_view(
		ID3D12Resource* resource,
		uint32_t count,
		size_t element_size);
	void destroy_buffer_view(st_dx12_descriptor offset);
	st_dx12_descriptor create_unordered_access_view(
		ID3D12Resource* resource,
		e_st_format format);
	void destroy_unordered_access_view(st_dx12_descriptor offset);

	void destroy_target(st_dx12_descriptor target);

	D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissor_rect;

	std::unique_ptr<class st_render_texture> _present_target;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> _swap_chain;
	Microsoft::WRL::ComPtr<ID3D12Device> _device;
	Microsoft::WRL::ComPtr<ID3D12Resource> _backbuffers[k_max_frames];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _command_allocators[k_max_frames];
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _command_queue;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _graphics_signature;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _compute_signature;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _command_list;

	// Heap used for loaded resources.
	std::unique_ptr<st_dx12_descriptor_heap> _resource_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _rtv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _dsv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _static_sampler_heap;

	// Heap used for drawing. Descriptors are copied to blocks in this heap with each draw.
	std::unique_ptr<st_dx12_descriptor_heap> _cbv_srv_heap[k_max_frames];
	std::unique_ptr<st_dx12_descriptor_heap> _sampler_heap[k_max_frames];

	std::unique_ptr<st_dx12_descriptor_heap> _gui_srv_heap;

	// Data upload heap.
	Microsoft::WRL::ComPtr<ID3D12Resource> _upload_buffer;
	void* _upload_buffer_head = nullptr;
	size_t _upload_buffer_offset = 0;

	// Dynamic geometry buffers.
	Microsoft::WRL::ComPtr<ID3D12Resource> _dynamic_vertex_buffer;
	size_t _dynamic_vertex_bytes_written = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> _dynamic_index_buffer;
	size_t _dynamic_index_bytes_written = 0;

	// Synchronization objects.
	HANDLE _fence_event;
	Microsoft::WRL::ComPtr<ID3D12Fence> _fence;
	uint64_t _fence_value = 0;

	// State tracking.
	float _clear_color[4] = { 0 };
	uint32_t _frame_index = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE _bound_targets[8] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE _bound_depth_stencil;
};

#endif
