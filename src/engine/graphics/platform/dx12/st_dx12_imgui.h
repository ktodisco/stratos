#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

class st_dx12_imgui
{
public:
	static void initialize(
		const class st_window* window,
		const class st_render_context* context);
	static void shutdown();

	static void new_frame();
	static void draw();
};

#endif
