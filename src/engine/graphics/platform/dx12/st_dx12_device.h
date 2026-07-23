#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>

class st_dx12_device : public st_device
{
public:

	st_dx12_device(ID3D12Device* device, class st_dx12_graphics_context* context);
	~st_dx12_device();

	// Commands.
	std::unique_ptr<class st_command_queue> create_command_queue(const st_command_queue_desc& desc) override;
	std::unique_ptr<class st_command_allocator> create_command_allocator(const st_command_allocator_desc& desc) override;
	std::unique_ptr<class st_command_list> create_command_list(const st_command_list_desc& desc) override;

	// Synchronization.
	std::unique_ptr<struct st_fence> create_fence(const st_fence_desc& desc) override;
	void wait(struct st_fence* fence, uint64_t value) override;

	// Swap chain.
	std::unique_ptr<st_swap_chain> create_swap_chain(const st_swap_chain_desc& desc) override;
	void reconfigure_swap_chain(const st_swap_chain_desc& desc, st_swap_chain* swap_chain) override;
	st_texture* get_backbuffer(st_swap_chain* swap_chain, uint32_t index) override;
	st_texture_view* get_backbuffer_view(st_swap_chain* swap_chain, uint32_t index) override;
	uint32_t get_backbuffer_index(st_swap_chain* swap_chain) override;
	e_st_swap_chain_status acquire_backbuffer(st_swap_chain* swap_chain) override;

	// Textures.
	std::unique_ptr<st_texture> create_texture(const st_texture_desc& desc) override;
	void set_texture_name(st_texture* texture, std::string name) override;
	std::unique_ptr<st_texture_view> create_texture_view(const st_texture_view_desc& desc) override;

	// Samplers.
	std::unique_ptr<st_sampler> create_sampler(const st_sampler_desc& desc) override;

	// Buffers.
	std::unique_ptr<st_buffer> create_buffer(const st_buffer_desc& desc) override;
	std::unique_ptr<st_buffer_view> create_buffer_view(const st_buffer_view_desc& desc) override;
	void map(st_buffer* buffer, uint32_t subresource, const st_range& range, void** outData) override;
	void unmap(st_buffer* buffer, uint32_t subresource, const st_range& range) override;
	void set_buffer_name(st_buffer* buffer, std::string name) override;

	// Resource tables.
	// TODO: This would take a root signature object. In the vk backend it would pull the descriptor layouts
	// from that root signature.
	// I especially don't like this because there isn't any barrier to forgetting to use one or the other.
	// The new API would look like a create and a generic set_resources which has overloads for texture
	// and buffer views, and each takes the root signature and the slot the resources are going to.
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

	// Shaders.
	std::unique_ptr<st_shader> create_shader(const char* filename, e_st_shader_type_flags type) override;

	// Pipelines.
	std::unique_ptr<st_pipeline> create_graphics_pipeline(const struct st_graphics_state_desc& desc) override;
	std::unique_ptr<st_pipeline> create_compute_pipeline(const struct st_compute_state_desc& desc) override;

	// Geometry.
	std::unique_ptr<st_vertex_format> create_vertex_format(
		const struct st_vertex_attribute* attributes,
		uint32_t attribute_count) override;

	// Render passes.
	std::unique_ptr<st_render_pass> create_render_pass(const st_render_pass_desc& desc) override;

	// Framebuffers.
	std::unique_ptr<st_framebuffer> create_framebuffer(const st_framebuffer_desc& desc) override;

	// Informational.
	void get_desc(const st_texture* texture, st_texture_desc* out_desc) override;

	// Internal functions.
	ID3D12Device* get() { return _d3d_device.Get(); }
	st_dx12_descriptor_heap* get_resource_heap() { return _resource_heap.get(); }
	st_dx12_descriptor_heap* get_rtv_heap() { return _rtv_heap.get(); }
	st_dx12_descriptor_heap* get_dsv_heap() { return _dsv_heap.get(); }
	st_dx12_descriptor_heap* get_static_sampler_heap() { return _static_sampler_heap.get(); }
	ID3D12RootSignature* get_graphics_layout() { return _graphics_signature.Get(); }
	ID3D12RootSignature* get_compute_layout() { return _compute_signature.Get(); }

private:

	void create_buffer_internal(size_t size, ID3D12Resource** resource);

	Microsoft::WRL::ComPtr<ID3D12Device> _d3d_device;

	class st_dx12_graphics_context* _context;

	// TODO: Delete these.
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _graphics_signature;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _compute_signature;

	// Heap used for loaded resources.
	std::unique_ptr<st_dx12_descriptor_heap> _resource_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _rtv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _dsv_heap;
	std::unique_ptr<st_dx12_descriptor_heap> _static_sampler_heap;
};

#endif
