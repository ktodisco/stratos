#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

class st_vk_command_list : public st_command_list
{
public:

	st_vk_command_list(vk::CommandBuffer* command_buffer);
	~st_vk_command_list();

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

private:

	vk::CommandBuffer* _command_buffer;
};

#endif
