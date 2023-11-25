#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <string>

class st_render_marker
{
public:

	st_render_marker(st_render_context* context, const std::string& marker) :
		_context(context)
	{
		_context->begin_marker(marker);
	}

	~st_render_marker()
	{
		_context->end_marker();
	}

private:
	st_render_context* _context;
};
