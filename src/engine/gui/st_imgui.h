#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <imgui.h>

#include <graphics/st_graphics.h>

class st_imgui
{
public:
	static void initialize(
		const class st_window* window,
		class st_graphics_context* context);
	static void shutdown();

	static void update(class st_sim* sim);

	static void new_frame();
	static void draw();

private:
	static std::unique_ptr<st_render_pass> _render_pass;
	static st_graphics_context* _context;

	static bool _open;
};
