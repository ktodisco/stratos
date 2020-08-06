#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/st_render_texture.h>

#include <math/st_vec4f.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// TODO: Find an ideal way to represent these.
#define k_max_shader_resources 1024
#define k_max_samplers 1024

template<typename T>
static T align_value(T value, uint32_t alignment)
{
	return (value + ((T)alignment - 1)) & ~((T)alignment - 1);
}

class st_vk_render_context
{
public:

	st_vk_render_context(const class st_window* window);
	~st_vk_render_context();

	void acquire() {}
	void release() {}

	void set_pipeline_state(const class st_vk_pipeline_state* state);
	void set_viewport(int x, int y, int width, int height) {}
	void set_scissor(int left, int top, int right, int bottom) {}
	void set_clear_color(float r, float g, float b, float a) {}

	void set_shader_resource_table(const vk::DescriptorSet& set);
	void set_sampler_table(const vk::DescriptorSet& set);
	void set_constant_buffer_table(const vk::DescriptorSet& set);
	void set_buffer_table(const vk::DescriptorSet& set);

	void set_render_targets(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil) {}

	void clear(unsigned int clear_flags) {}
	void draw(const struct st_static_drawcall& drawcall);
	void draw(const struct st_dynamic_drawcall& drawcall) {}

	// TODO: These are temporary and a generic solution is needed.
	void transition_backbuffer_to_target();
	void transition_backbuffer_to_present();

	/*void transition_targets(
		uint32_t count,
		D3D12_RESOURCE_BARRIER* barriers) {}*/

	void begin_frame();
	void end_frame();
	void swap();

	void begin_marker(const std::string& marker) {}
	void end_marker() {}

	// TODO: Add a new argument for the initial state of the texture.
	void create_texture(
		uint32_t width,
		uint32_t height,
		uint32_t mip_count,
		e_st_format format,
		e_st_texture_usage_flags usage,
		vk::Image& resource);
	void destroy_texture(vk::Image& resource);
	void create_texture_view(class st_vk_texture* texture, vk::ImageView& resource);
	void destroy_texture_view(vk::ImageView& resource);
	void upload_texture(class st_vk_texture* texture, void* data);
	void create_buffer(size_t size, e_st_buffer_usage_flags usage, vk::Buffer& resource);
	void update_buffer(vk::Buffer& resource, size_t offset, size_t num_bytes, const void* data);
	void destroy_buffer(vk::Buffer& resource);
	void create_descriptor_set(e_st_descriptor_slot slot, vk::DescriptorSet* set);
	void destroy_descriptor_set(vk::DescriptorSet& set);
	void create_sampler(vk::Sampler& sampler);
	void destroy_sampler(vk::Sampler& sampler);

	vk::Device* get_device() { return std::addressof(_device); }
	vk::CommandBuffer* get_command_buffer() { return std::addressof(_command_buffers[st_command_buffer_graphics]); }
	vk::PipelineLayout* get_layout() { return std::addressof(_pipeline_layout); }
	const st_render_texture* get_present_target() { return _present_target.get(); }

	static st_vk_render_context* get() { return _this; }

private:

	static const uint32_t k_backbuffer_count = 2;

	vk::SurfaceKHR _window_surface;
	vk::Image _backbuffers[k_backbuffer_count];
	vk::SwapchainKHR _swap_chain;

	// This texture is used by the application as a proxy for the double-buffered backbuffer.
	// This prevents the implementation from needing to create two framebuffer objects for
	// each of the backbuffer images and having the double buffering bleed into the first-
	// class render passes, especially depending on which ones render to the backbuffer.
	std::unique_ptr<class st_render_texture> _present_target;

	// Maintain a global instance.
	static st_vk_render_context* _this;

	vk::Instance _instance;
	vk::PhysicalDevice _gpu;
	vk::Device _device;
	vk::Queue _queue;

	uint32_t _device_memory_index = UINT_MAX;

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

	uint32_t _queue_family_index = UINT_MAX;

	vk::DescriptorSetLayout _descriptor_layouts[st_descriptor_slot_count];
	vk::PipelineLayout _pipeline_layout;
	vk::DescriptorPool _descriptor_pool;

	/*D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissor_rect;

	// Dynamic geometry buffers.
	Microsoft::WRL::ComPtr<ID3D12Resource> _dynamic_vertex_buffer;
	size_t _dynamic_vertex_bytes_written = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> _dynamic_index_buffer;
	size_t _dynamic_index_bytes_written = 0;

	// State tracking.
	float _clear_color[4] = { 0 };
	uint32_t _frame_index = 0;*/
};

#endif
