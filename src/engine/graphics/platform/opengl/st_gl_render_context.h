#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <memory>

class st_gl_render_context
{
public:

	st_gl_render_context(const class st_window* window);
	~st_gl_render_context();

	void acquire();
	void release();

	void set_pipeline_state(const class st_gl_pipeline_state* state);
	void set_viewport(const st_gl_viewport& viewport);
	void set_scissor(int left, int top, int right, int bottom);
	void set_clear_color(float r, float g, float b, float a);

	void clear(unsigned int clear_flags);
	void draw(const struct st_gl_static_drawcall& drawcall);
	void draw(const struct st_gl_dynamic_drawcall& drawcall);

	// TODO: These are temporary and a generic solution is needed.
	void transition_backbuffer_to_target();
	void transition_backbuffer_to_present();

	void begin_frame();
	void end_frame();
	void swap();

	const class st_gl_shader* get_bound_shader() const { return _bound_shader; }
	class st_render_texture* get_present_target() const { return _present_target.get(); }

	static st_gl_render_context* get() { return _this; }

private:
	void set_depth_state(bool enable, GLenum func);
	void set_cull_state(bool enable, GLenum mode);
	void set_blend_state(bool enable, GLenum src_factor, GLenum dst_factor);
	void set_depth_mask(bool enable);

private:
	// Maintain a global instance.
	static st_gl_render_context* _this;

	std::unique_ptr<class st_render_texture> _present_target;
	std::unique_ptr<class st_gl_framebuffer> _present_framebuffer;

	HDC _device_context;
	HGLRC _gl_context;

	float _clear_color[4] = { 0 };

	const class st_gl_shader* _bound_shader = nullptr;
};

#endif
