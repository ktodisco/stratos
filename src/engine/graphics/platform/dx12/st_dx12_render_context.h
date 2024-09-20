#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/st_render_context.h>

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>

#include <math/st_vec4f.h>

#include <cstdint>
#include <string>

// TODO: Find an ideal way to represent these.
#define k_max_shader_resources 1024
#define k_max_samplers 1024

class st_dx12_render_context : public st_render_context
{
public:

	st_dx12_render_context(const class st_window* window);
	~st_dx12_render_context();

	void acquire() override;
	void release() override;

	void set_pipeline(const st_pipeline* state) override;
	void set_viewport(const st_viewport& viewport) override;
	void set_scissor(int left, int top, int right, int bottom) override;
	void set_clear_color(float r, float g, float b, float a) override;

	void set_render_targets(
		uint32_t count,
		const st_texture_view** targets,
		const st_texture_view* depth_stencil) override;

	void clear(unsigned int clear_flags) override;
	void draw(const struct st_static_drawcall& drawcall) override;
	void draw(const struct st_dynamic_drawcall& drawcall) override;

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
	std::unique_ptr<st_texture> create_texture(
		uint32_t width,
		uint32_t height,
		uint32_t levels,
		e_st_format format,
		e_st_texture_usage_flags usage,
		e_st_texture_state initial_state,
		const st_vec4f& clear,
		void* data) override;
	void set_texture_meta(st_texture* texture, const char* name) override;
	void set_texture_name(st_texture* texture, std::string name) override;
	void transition(
		st_texture* texture,
		e_st_texture_state new_state) override;
	std::unique_ptr<st_texture_view> create_texture_view(st_texture* texture) override;

	// Buffers.
	std::unique_ptr<st_buffer> create_buffer(
		const uint32_t count,
		const size_t element_size,
		const e_st_buffer_usage_flags usage) override;
	std::unique_ptr<st_buffer_view> create_buffer_view(st_buffer* buffer) override;
	void map(st_buffer* buffer, uint32_t subresource, const st_range& range, void** outData) override;
	void unmap(st_buffer* buffer, uint32_t subresource, const st_range& range) override;
	void update_buffer(st_buffer* buffer, void* data, const uint32_t count) override;
	void set_buffer_meta(st_buffer* buffer, std::string name) override;

	// Constant buffers.
	void add_constant(
		st_buffer* buffer,
		const std::string& name,
		const e_st_shader_constant_type constant_type) override;

	// Resource tables.
	std::unique_ptr<st_resource_table> create_resource_table() override;
	void set_constant_buffers(st_resource_table* table, uint32_t count, st_buffer** cbs) override;
	void set_textures(st_resource_table* table, uint32_t count, st_texture** textures) override;
	void set_buffers(st_resource_table* table, uint32_t count, st_buffer** buffers) override;
	void bind_resource_table(st_resource_table* table) override;

	// Shaders.
	std::unique_ptr<st_shader> create_shader(const char* filename, uint8_t type) override;

	// Pipelines.
	std::unique_ptr<st_pipeline> create_pipeline(
		const struct st_pipeline_state_desc& desc,
		const struct st_render_pass* render_pass) override;

	// Geometry.
	std::unique_ptr<st_vertex_format> create_vertex_format(
		const st_vertex_attribute* attributes,
		uint32_t attribute_count) override;
	std::unique_ptr<st_geometry> create_geometry(
		const st_vertex_format* format,
		void* vertex_data,
		uint32_t vertex_size,
		uint32_t vertex_count,
		uint16_t* index_data,
		uint32_t index_count) override;

	// Render passes.
	std::unique_ptr<st_render_pass> create_render_pass(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil) override;
	void begin_render_pass(
		st_render_pass* pass,
		st_vec4f* clear_values,
		const uint8_t clear_count) override;
	void end_render_pass(st_render_pass* pass) override;

	// Informational.
	e_st_graphics_api get_api() { return e_st_graphics_api::dx12; }

	// API-specific.
	ID3D12Device* get_device() const { return _device.Get(); }
	ID3D12GraphicsCommandList* get_command_list() const { return _command_list.Get(); }
	st_dx12_descriptor_heap* get_gui_heap() const { return _gui_srv_heap.get(); }

	// TODO: Collapse these into bind_resource_table as a lambda.
	void set_shader_resource_table(uint32_t offset);
	void set_sampler_table(uint32_t offset);
	void set_constant_buffer_table(uint32_t offset);
	void set_buffer_table(uint32_t offset);

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
	st_dx12_descriptor create_shader_sampler();
	void destroy_shader_sampler(st_dx12_descriptor offset);
	// TODO: Combine this with create_shader_resource_view.
	st_dx12_descriptor create_buffer_view(
		ID3D12Resource* resource,
		uint32_t count,
		size_t element_size);
	void destroy_buffer_view(st_dx12_descriptor offset);

	void destroy_target(st_dx12_descriptor target);

	static const uint32_t k_backbuffer_count = 2;

	D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissor_rect;

	std::unique_ptr<class st_render_texture> _present_target;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> _swap_chain;
	Microsoft::WRL::ComPtr<ID3D12Device> _device;
	Microsoft::WRL::ComPtr<ID3D12Resource> _backbuffers[k_backbuffer_count];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _command_allocators[k_backbuffer_count];
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _command_queue;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _root_signature;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _command_list;

	std::unique_ptr<st_dx12_descriptor_heap> _rtv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _dsv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _cbv_srv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _sampler_heap;

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
