#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/geometry/st_geometry.h>

#include <memory>

class st_fullscreen_render_pass
{
public:
	st_fullscreen_render_pass();
	~st_fullscreen_render_pass();

protected:
	std::unique_ptr<struct st_vertex_format> _vertex_format = nullptr;
	std::unique_ptr<class st_geometry> _fullscreen_quad = nullptr;

	std::unique_ptr<class st_material> _material = nullptr;
	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;
};
