#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

class st_gl_command_allocator : public st_command_allocator
{
public:

	st_gl_command_allocator();
	~st_gl_command_allocator();

	void reset() override;
};

#endif
