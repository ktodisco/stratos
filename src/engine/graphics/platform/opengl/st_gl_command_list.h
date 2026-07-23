#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

class st_gl_command_list : public st_command_list
{
public:

	st_gl_command_list(class st_gl_device* device);
	~st_gl_command_list();

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
	// TODO: This is a band-aid.
	// Both modern APIs follow the same pattern: transfer the data to a gpu-readable
	// upload buffer of device-created resources, then issue copy commands on the
	// command list to transfer this to the resource. The goal would be to break this
	// out into parts that are possible to do from above the platform api level.
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
	void set_depth_state(bool enable, GLenum func);
	void set_stencil_state(const st_depth_stencil_desc& desc);
	void set_cull_state(bool enable, GLenum mode);
	void set_blend_state(bool enable, GLenum src_factor, GLenum dst_factor);
	void set_depth_mask(bool enable);

	// Active draw state.
	const class st_gl_shader* _bound_shader = nullptr;
	const struct st_gl_pipeline* _bound_pipeline = nullptr;

	class st_gl_device* _device = nullptr;
};

#endif
