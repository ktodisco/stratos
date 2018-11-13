#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>

#include <math/st_vec4f.h>

#include <cstdint>
#include <string>

#define k_st_depth_less 0

#define k_st_src_alpha 0
#define k_st_one_minus_src_alpha 0

// TODO: Find an ideal way to represent these.
#define k_max_shader_resources 1024
#define k_max_samplers 1024

template<typename T>
static T align_value(T value, uint32_t alignment)
{
	return (value + ((T)alignment - 1)) & ~((T)alignment - 1);
}

class st_dx12_render_context
{
public:

	st_dx12_render_context(const class st_window* window);
	~st_dx12_render_context();

	void acquire();
	void release();

	void set_pipeline_state(const class st_dx12_pipeline_state* state);
	void set_viewport(int x, int y, int width, int height);
	void set_scissor(int left, int top, int right, int bottom);
	void set_clear_color(float r, float g, float b, float a);

	void set_shader_resource_table(uint32_t offset);
	void set_sampler_table(uint32_t offset);
	void set_constant_buffer_table(uint32_t offset);

	void set_render_targets(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil);

	void clear(unsigned int clear_flags);
	void draw(const struct st_static_drawcall& drawcall);
	void draw(const struct st_dynamic_drawcall& drawcall);

	// TODO: These are temporary and a generic solution is needed.
	void transition_backbuffer_to_target();
	void transition_backbuffer_to_present();

	void transition_targets(
		uint32_t count,
		D3D12_RESOURCE_BARRIER* barriers);

	void begin_loading();
	void end_loading();
	void begin_frame();
	void end_frame();
	void swap();

	void begin_marker(const std::string& marker);
	void end_marker();

	void create_graphics_pipeline_state(
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline_desc,
		ID3D12PipelineState** pipeline_state);
	void create_buffer(size_t size, ID3D12Resource** resource);
	void create_texture(
		uint32_t width,
		uint32_t height,
		e_st_texture_format format,
		void* data,
		ID3D12Resource** resource,
		uint32_t* sampler_offset,
		uint32_t* srv_offset);
	void create_target(
		uint32_t width,
		uint32_t height,
		e_st_texture_format format,
		const st_vec4f& clear,
		ID3D12Resource** resource,
		st_dx12_descriptor* rtv_offset);
	void destroy_target(st_dx12_descriptor target);

	st_dx12_descriptor create_constant_buffer_view(
		D3D12_GPU_VIRTUAL_ADDRESS gpu_address,
		size_t size);
	void destroy_constant_buffer_view(st_dx12_descriptor offset);
	st_dx12_descriptor create_shader_resource_view(
		ID3D12Resource* resource,
		e_st_texture_format format);
	void destroy_shader_resource_view(st_dx12_descriptor offset);
	st_dx12_descriptor create_shader_sampler();
	void destroy_shader_sampler(st_dx12_descriptor offset);

	ID3D12RootSignature* get_root_signature() { return _root_signature.Get(); }

	static st_dx12_render_context* get();

private:

	static const uint32_t k_backbuffer_count = 2;

	// Maintain a global instance.
	static st_dx12_render_context* _this;

	D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissor_rect;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> _swap_chain;
	Microsoft::WRL::ComPtr<ID3D12Device> _device;
	Microsoft::WRL::ComPtr<ID3D12Resource> _backbuffers[k_backbuffer_count];
	Microsoft::WRL::ComPtr<ID3D12Resource> _depth_stencil;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _command_allocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _command_queue;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _root_signature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _default_pso;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _command_list;

	std::unique_ptr<st_dx12_descriptor_heap> _rtv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _dsv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _cbv_srv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _sampler_heap;

	// Data upload heap.
	Microsoft::WRL::ComPtr<ID3D12Resource> _upload_buffer;
	uint8_t* _upload_buffer_start = 0;

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
