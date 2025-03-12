#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>

#include <math/st_vec3f.h>
#include <math/st_vec4f.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class st_vk_graphics_context : public st_graphics_context
{
public:

	st_vk_graphics_context(const class st_window* window);
	~st_vk_graphics_context();

	void acquire() override {}
	void release() override {}

	void set_pipeline(const st_pipeline* state) override;
	void set_compute_pipeline(const st_pipeline* state) override;
	void set_viewport(const st_viewport& viewport) override;
	void set_scissor(int left, int top, int right, int bottom) override;
	void set_blend_factor(float r, float g, float b, float a) override {}

	void draw(const struct st_static_drawcall& drawcall) override;
	void draw(const struct st_dynamic_drawcall& drawcall) override;

	// Compute.
	void dispatch(const st_dispatch_args& args) override;

	// Swap chain.
	std::unique_ptr<st_swap_chain> create_swap_chain(const st_swap_chain_desc& desc) override;
	void reconfigure_swap_chain(const st_swap_chain_desc& desc, st_swap_chain* swap_chain) override;
	st_texture* get_backbuffer(st_swap_chain* swap_chain, uint32_t index) override;
	st_texture_view* get_backbuffer_view(st_swap_chain* swap_chain, uint32_t index) override;
	void acquire_backbuffer(st_swap_chain* swap_chain) override;

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
		const struct st_vertex_attribute* attributes,
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
	e_st_graphics_api get_api() { return e_st_graphics_api::vulkan; }
	void get_desc(const st_texture* texture, st_texture_desc* out_desc) override;

	// API-specific.
	vk::Device* get_device() { return std::addressof(_device); }

private:

	void create_swap_chain_internal(const st_swap_chain_desc& desc, st_vk_swap_chain* swap_chain);

	vk::Instance _instance;
	vk::PhysicalDevice _gpu;
	vk::PhysicalDeviceProperties _caps;
	vk::Device _device;
	vk::Queue _queue;

	uint32_t _device_memory_index = UINT_MAX;
	uint32_t _mapped_memory_index = UINT_MAX;

	enum e_st_command_buffer
	{
		st_command_buffer_loading,
		st_command_buffer_graphics,

		st_command_buffer_count
	};

	vk::CommandPool _command_pools[st_command_buffer_count];
	vk::CommandBuffer _command_buffers[st_command_buffer_count];

	vk::Fence _acquire_fence;
	vk::Fence _fence;

	vk::Buffer _upload_buffer;
	vk::DeviceMemory _upload_buffer_memory;
	void* _upload_buffer_head;
	size_t _upload_buffer_offset;

	uint32_t _queue_family_index = UINT_MAX;

	vk::DescriptorSetLayout _graphics_layouts[st_descriptor_slot_count];
	vk::DescriptorSetLayout _compute_layouts[st_descriptor_slot_count];
	vk::PipelineLayout _graphics_signature;
	vk::PipelineLayout _compute_signature;
	vk::DescriptorPool _descriptor_pool;

	std::vector<vk::DescriptorSet> _descriptor_set_pool[k_max_frames];
	uint32_t _descriptor_set_index = 0;

	// Dynamic geometry buffers.
	struct st_vk_procedural_vertex
	{
		struct st_vec3f _pos;
		struct st_vec3f _color;
	};
	std::unique_ptr<st_buffer> _dynamic_vertex_buffer = nullptr;
	std::unique_ptr<st_buffer> _dynamic_index_buffer = nullptr;
	uint32_t _dynamic_vertex_bytes_written = 0;
	uint32_t _dynamic_index_bytes_written = 0;

	void* _vk_library = nullptr;
	bool _has_markers = false;
};

#endif
