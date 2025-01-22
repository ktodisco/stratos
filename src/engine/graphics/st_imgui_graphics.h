#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

class st_imgui_graphics
{
public:

	static void initialize(
		const class st_window* window,
		const class st_graphics_context* context) {}
	static void shutdown() {}

	static void new_frame() {}
	static void draw() {}
};
