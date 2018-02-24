#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <string>

class st_gl_render_marker
{
public:
	st_gl_render_marker(const std::string& message);
	~st_gl_render_marker();
};

#endif
