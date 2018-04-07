#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_geometry.h>
#include <graphics/st_material.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_vertex_format.h>

#include <memory>

class st_fullscreen_render_pass
{
public:
	st_fullscreen_render_pass();
	~st_fullscreen_render_pass();

protected:
	std::unique_ptr<st_vertex_format> _vertex_format = nullptr;
	std::unique_ptr<st_geometry> _fullscreen_quad = nullptr;

	std::unique_ptr<st_material> _material = nullptr;
	std::unique_ptr<st_pipeline_state> _pipeline_state = nullptr;
};
