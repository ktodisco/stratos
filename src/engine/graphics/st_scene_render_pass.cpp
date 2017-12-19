/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_scene_render_pass.h"

#include "framework/st_frame_params.h"
#include "graphics/st_drawcall.h"
#include "graphics/st_material.h"
#include "graphics/st_render_context.h"
#include "graphics/st_render_marker.h"

st_scene_render_pass::st_scene_render_pass()
{
	_default_material = new st_constant_color_material();
	_default_material->init();
}

st_scene_render_pass::~st_scene_render_pass()
{
	delete _default_material;
}

void st_scene_render_pass::render(st_render_context* context, const st_frame_params* params)
{
	st_render_marker marker("st_scene_render_pass::render");

	// Compute projection matrices:
	st_mat4f perspective;
	perspective.make_perspective_rh(st_degrees_to_radians(45.0f), (float)params->_width / (float)params->_height, 0.1f, 10000.0f);

	// Draw all static geometry:
	for (auto& d : params->_static_drawcalls)
	{
		st_render_marker draw_marker(d._name.c_str());

		d._material->bind(context, perspective, params->_view, d._transform);
		context->draw(d);
	}

	// Draw all dynamic geometry:
	draw_dynamic(context, params->_dynamic_drawcalls, perspective, params->_view);
}

void st_scene_render_pass::draw_dynamic(
	st_render_context* context,
	const std::vector<st_dynamic_drawcall>& drawcalls,
	const class st_mat4f& proj,
	const class st_mat4f& view)
{
	for (auto& d : drawcalls)
	{
		st_render_marker draw_marker(d._name);

		if (d._material)
		{
			d._material->set_color(d._color);
			d._material->bind(context, proj, view, d._transform);
		}
		else
		{
			_default_material->set_color(d._color);
			_default_material->bind(context, proj, view, d._transform);
		}

		context->draw(d);
	}
}
