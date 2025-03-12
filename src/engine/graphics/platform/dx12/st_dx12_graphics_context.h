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
	void set_blend_factor(float r, float g, float b, float a) override;

	void draw(const struct st_static_drawcall& drawcall) override;
	void draw(const struct st_dynamic_drawcall& drawcall) override;

	// Compute.
	void dispatch(const st_dispatch_args& args) override;

	// Swap chain.
	std::unique_ptr<st_swap_chain> create_swap_chain(const st_swap_chain_desc& desc) override;
	void reconfigure_swap_chain(const st_swap_chain_desc& desc, st_swap_chain* swap_chain) override;
	st_texture* get_backbuffer(st_swap_chain* swap_chain, uint32_t index) override;
	st_texture_view* get_backbuffer_view(st_swap_chain* swap_chain, uint32_t index) override;
	void acquire_backbuffer(st_swap_chain* swap_chain) override {}

	void begin_loading() override;
	void end_loading() override;
	void begin_frame() override;
	void end_frame() override;
	void execute() override;
	void present(st_swap_chain* swap_chain) override;
	void wait_for_idle() override;

	void begin_marker(const std::string& marker) override;
	void end_marker() override;

	// Textures.
	// TODO: Fast clear value.
	std::unique_ptr<st_texture> create_texture(const st_texture_desc& desc) override;
	void set_texture_name(st_texture* texture, std::string name) override;
	void transition(
		st_texture* texture,
		e_st_texture_state new_state) override;
	std::unique_ptr<st_texture_view> create_texture_view(const st_texture_view_desc& desc) override;

	// Samplers.
	std::unique_ptr<st_sampler> create_sampler(const st_sampler_desc& desc) override;

	// Buffers.
	std::unique_ptr<st_buffer> create_buffer(const st_buffer_desc& desc) override;
	std::unique_ptr<st_buffer_view> create_buffer_view(const st_buffer_view_desc& desc) override;
	void map(st_buffer* buffer, uint32_t subresource, const st_range& range, void** outData) override;
	void unmap(st_buffer* buffer, uint32_t subresource, const st_range& range) override;
	void update_buffer(st_buffer* buffer, void* data, const uint32_t offset, const uint32_t count) override;
	void set_buffer_name(st_buffer* buffer, std::string name) override;

	// Resource tables.
	std::unique_ptr<st_resource_table> create_resource_table() override;
	std::unique_ptr<st_resource_table> create_resource_table_compute() override;
	void set_constant_buffers(st_resource_table* table, uint32_t count, const st_buffer_view** cbs) override;
	void set_textures(
		st_resource_table* table,
		uint32_t count,
		const st_texture_view** textures,
		const st_sampler** samplers) override;
	void set_buffers(st_resource_table* table, uint32_t count, const st_buffer_view** buffers) override;
	void set_uavs(st_resource_table* table, uint32_t count, const st_texture_view** textures) override;
	void update_textures(st_resource_table* table, uint32_t count, const st_texture_view** views) override;
	void bind_resources(st_resource_table* table) override;
	void bind_compute_resources(st_resource_table* table) override;

	// Shaders.
	std::unique_ptr<st_shader> create_shader(const char* filename, e_st_shader_type_flags type) override;

	// Pipelines.
	std::unique_ptr<st_pipeline> create_graphics_pipeline(const struct st_graphics_state_desc& desc) override;
	std::unique_ptr<st_pipeline> create_compute_pipeline(const struct st_compute_state_desc& desc) override;

	// Geometry.
	std::unique_ptr<st_vertex_format> create_vertex_format(
		const st_vertex_attribute* attributes,
		uint32_t attribute_count) override;

	// Render passes.
	std::unique_ptr<st_render_pass> create_render_pass(const st_render_pass_desc& desc) override;
	void begin_render_pass(
		st_render_pass* pass,
		st_framebuffer* framebuffer,
		const st_clear_value* clear_values,
		const uint8_t clear_count) override;
	void end_render_pass(st_render_pass* pass, st_framebuffer* framebuffer) override;

	// Framebuffers.
	std::unique_ptr<st_framebuffer> create_framebuffer(const st_framebuffer_desc& desc) override;

	// Informational.
	e_st_graphics_api get_api() { return e_st_graphics_api::dx12; }
	void get_desc(const st_texture* texture, st_texture_desc* out_desc) override;

	// API-specific.
	ID3D12Device* get_device() const { return _device.Get(); }
	ID3D12GraphicsCommandList* get_command_list() const { return _command_list.Get(); }
	st_dx12_descriptor_heap* get_gui_heap() const { return _gui_srv_heap.get(); }

private:

	void set_render_targets(
		uint32_t count,
		st_texture_view** targets,
		st_texture_view* depth_stencil);

	void create_buffer_internal(size_t size, ID3D12Resource** resource);
	void destroy_target(st_dx12_descriptor target);

	D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissor_rect;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> _swap_chain;
	Microsoft::WRL::ComPtr<IDXGIFactory4> _dxgi_factory;
	Microsoft::WRL::ComPtr<ID3D12Device> _device;
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

	D3D12_CPU_DESCRIPTOR_HANDLE _bound_targets[8] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE _bound_depth_stencil;
};

#endif
