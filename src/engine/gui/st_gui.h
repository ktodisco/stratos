#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <imgui.h>

class st_gui
{
public:
	static void initialize(
		const class st_window* window,
		const class st_render_context* context);
	static void shutdown();

	static void new_frame();
	static void draw();
};
