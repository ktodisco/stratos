/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_scene_render_pass.h"

#include "framework/st_frame_params.h"
#include "graphics/st_drawcall.h"
#include "graphics/st_material.h"
#include "graphics/st_pipeline_state.h"
#include "graphics/st_render_context.h"
#include "graphics/st_render_marker.h"

st_scene_render_pass::st_scene_render_pass()
{
	_default_material = std::make_unique<st_phong_material>();

	st_pipeline_state_desc state_desc;
	_default_material->get_pipeline_state(&state_desc);

	state_desc._render_target_count = 1;
	state_desc._render_target_formats[0] = st_texture_format_r8g8b8a8_unorm;
	state_desc._depth_stencil_format = st_texture_format_d24_unorm_s8_uint;

	_pipeline_state = std::make_unique<st_pipeline_state>(state_desc);
}

st_scene_render_pass::~st_scene_render_pass()
{
}

void st_scene_render_pass::render(st_render_context* context, const st_frame_params* params)
{
	st_render_marker marker("st_scene_render_pass::render");

	// Compute projection matrices:
	st_mat4f perspective;
	perspective.make_perspective_rh(st_degrees_to_radians(45.0f), (float)params->_width / (float)params->_height, 0.1f, 10000.0f);

	context->set_pipeline_state(_pipeline_state.get());

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
			//d._material->set_color(d._color);
			d._material->bind(context, proj, view, d._transform);
		}
		else
		{
			//_default_material->set_color(d._color);
			_default_material->bind(context, proj, view, d._transform);
		}

		context->draw(d);
	}
}
