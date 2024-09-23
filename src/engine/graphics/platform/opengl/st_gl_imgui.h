#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

class st_gl_imgui
{
public:
	static void initialize(
		const class st_window* window,
		const class st_graphics_context* context);
	static void shutdown();

	static void new_frame();
	static void draw();
};

#endif
