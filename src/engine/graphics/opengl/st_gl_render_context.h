#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <Windows.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>

#define k_st_depth_less GL_LESS

#define k_st_src_alpha GL_SRC_ALPHA
#define k_st_one_minus_src_alpha GL_ONE_MINUS_SRC_ALPHA

class st_gl_render_context
{
public:

	st_gl_render_context(HDC device_context);
	~st_gl_render_context();

	HDC get_device_context() const
	{
		return _device_context;
	}

	void acquire();
	void release();

	void set_viewport(int x, int y, int width, int height);
	// TODO: These state functions should be combined into a single state call.
	void set_depth_state(bool enable, GLenum func);
	void set_cull_state(bool enable);
	void set_blend_state(bool enable, GLenum src_factor, GLenum dst_factor);
	void set_depth_mask(bool enable);
	void set_clear_color(float r, float g, float b, float a);

	void clear(unsigned int clear_flags);
	void draw(const struct st_static_drawcall& drawcall);
	void draw(const struct st_dynamic_drawcall& drawcall);

	void swap();

private:

	HDC _device_context;
	HGLRC _gl_context;

	float _clear_color[4] = { 0 };
};
