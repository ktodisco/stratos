/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_ui_render_pass.h"

#include "framework/st_frame_params.h"
#include "graphics/st_drawcall.h"
#include "graphics/st_material.h"
#include "graphics/st_render_context.h"
#include "graphics/st_render_marker.h"

st_ui_render_pass::st_ui_render_pass()
{
	_default_material = new st_constant_color_material();
	_default_material->init();
}

st_ui_render_pass::~st_ui_render_pass()
{
	delete _default_material;
}

void st_ui_render_pass::render(st_render_context* context, const st_frame_params* params)
{
	st_render_marker marker("st_ui_render_pass::render");

	st_mat4f ortho;
	ortho.make_orthographic(0.0f, (float)params->_width, (float)params->_height, 0.0f, 0.1f, 10000.0f);
	st_mat4f view;
	view.make_lookat_rh(st_vec3f::z_vector(), -st_vec3f::z_vector(), st_vec3f::y_vector());

	draw_dynamic(context, params->_gui_drawcalls, ortho, view);
}

void st_ui_render_pass::draw_dynamic(
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
