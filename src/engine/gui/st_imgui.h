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

	static void update(struct st_frame_params* params, class st_sim* sim);

	static void new_frame();
	static void draw();

private:
	static void draw_axes_widget(struct st_frame_params* params);

	static std::unique_ptr<st_render_pass> _render_pass;
	static st_graphics_context* _context;

	static bool _open;

	static bool _axes_widget;
};
