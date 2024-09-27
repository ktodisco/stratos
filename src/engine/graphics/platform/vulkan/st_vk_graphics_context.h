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
	void set_viewport(const st_viewport& viewport) override;
	void set_scissor(int left, int top, int right, int bottom) override;
	void set_clear_color(float r, float g, float b, float a) override {}

	void set_render_targets(
		uint32_t count,
		const st_texture_view** targets,
		const st_texture_view* depth_stencil) override {}

	void clear(unsigned int clear_flags) override {}
	void draw(const struct st_static_drawcall& drawcall) override;
	void draw(const struct st_procedural_drawcall& drawcall) override {}

	// Backbuffer.
	st_render_texture* get_present_target() const override;
	// TODO: These are temporary and a generic solution is needed.
	void transition_backbuffer_to_target() override;
	void transition_backbuffer_to_present() override;

	void begin_loading() override {}
	void end_loading() override {}
	void begin_frame() override;
	void end_frame() override;
	void swap() override;

	void begin_marker(const std::string& marker) override;
	void end_marker() override;

	// Textures.
	std::unique_ptr<st_texture> create_texture(
		uint32_t width,
		uint32_t height,
		uint32_t levels,
		e_st_format format,
		e_st_texture_usage_flags usage,
		e_st_texture_state initial_state,
		const st_vec4f& clear,
		void* data) override;
	void set_texture_meta(st_texture* texture, const char* name) override {}
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
	void update_buffer(st_buffer* buffer, void* data, const uint32_t offset, const uint32_t count) override;
	void set_buffer_meta(st_buffer* buffer, std::string name) override {}

	// TODO: In the unified architexture, create_buffer would take a base Buffer* and
	// the Vulkan implementation would contain both a vkBuffer and vkDeviceMemory.

	// Constant buffers.
	void add_constant(
		st_buffer* buffer,
		const std::string& name,
		const e_st_shader_constant_type constant_type) override {}

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
		const struct st_vertex_attribute* attributes,
		uint32_t attribute_count) override;

	// Render passes.
	std::unique_ptr<st_render_pass> create_render_pass(
		uint32_t count,
		struct st_target_desc* targets,
		struct st_target_desc* depth_stencil) override;
	void begin_render_pass(
		st_render_pass* pass,
		st_vec4f* clear_values,
		const uint8_t clear_count) override;
	void end_render_pass(st_render_pass* pass) override;

	// Informational.
	e_st_graphics_api get_api() { return e_st_graphics_api::vulkan; }

	// API-specific.
	vk::Device* get_device() { return std::addressof(_device); }

private:

	void create_sampler(vk::Sampler& sampler);
	void destroy_sampler(vk::Sampler& sampler);

	vk::SurfaceKHR _window_surface;
	vk::Image _backbuffers[k_max_frames];
	vk::SwapchainKHR _swap_chain;

	// This texture is used by the application as a proxy for the double-buffered backbuffer.
	// This prevents the implementation from needing to create two framebuffer objects for
	// each of the backbuffer images and having the double buffering bleed into the first-
	// class render passes, especially depending on which ones render to the backbuffer.
	std::unique_ptr<class st_render_texture> _present_target;

	// Maintain a global instance.
	static st_vk_graphics_context* _this;

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

	vk::DescriptorSetLayout _descriptor_layouts[st_descriptor_slot_count];
	vk::PipelineLayout _pipeline_layout;
	vk::DescriptorPool _descriptor_pool;

	std::vector<vk::DescriptorSet> _descriptor_set_pool[k_max_frames];
	uint32_t _descriptor_set_index = 0;

	uint32_t _frame_index = 0;

	void* _vk_library = nullptr;
	bool _has_markers = false;
};

#endif
