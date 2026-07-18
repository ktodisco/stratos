#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <math/st_vec3f.h>
#include <math/st_vec4f.h>

class st_vk_command_list : public st_command_list
{
public:

	st_vk_command_list(const struct st_command_list_desc& desc, class st_vk_device* device);
	~st_vk_command_list();

	void begin(class st_command_allocator* command_allocator) override;
	void end() override;

	void set_pipeline(const st_pipeline* pipeline) override;
	void set_compute_pipeline(const st_pipeline* pipeline) override;
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
	void upload(st_texture* texture, void* data) override;
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

	vk::CommandBuffer* get() { return &_command_buffer; }

private:

	vk::CommandBuffer _command_buffer;
	st_vk_device* _device;

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

	// Data upload heap.
	std::unique_ptr<st_buffer> _upload_buffer;
	void* _upload_buffer_head = nullptr;
	size_t _upload_buffer_offset = 0;

	uint32_t _frame_index = 0;
};

#endif
