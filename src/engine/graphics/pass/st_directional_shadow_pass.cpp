/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_directional_shadow_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/light/st_directional_light.h>
#include <graphics/material/st_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

#include <math/st_mat4f.h>
#include <math/st_vec3f.h>

st_directional_shadow_pass::st_directional_shadow_pass(st_render_texture* target) :
	_target(target)
{
	st_graphics_context* context = st_graphics_context::get();

	st_target_desc ds_target = { target, e_st_load_op::clear, e_st_store_op::store };
	_pass = context->create_render_pass(0, nullptr, &ds_target);
}

st_directional_shadow_pass::~st_directional_shadow_pass()
{
	_pass = nullptr;
}

void st_directional_shadow_pass::render(st_graphics_context* context, const st_frame_params* params)
{
	st_render_marker marker(context, "st_directional_shadow_pass::render");

	context->set_scissor(0, 0, _target->get_width(), _target->get_height());

	st_clear_value clears[] =
	{
		st_depth_stencil_clear_value{ 1.0f, 0 }
	};

	context->begin_render_pass(_pass.get(), clears, std::size(clears));
	context->clear(st_clear_flag_depth);

	for (auto& d : params->_static_drawcalls)
	{
		st_render_marker draw_marker(context, d._name.c_str());

		if (d._material->supports_pass(e_st_render_pass_type::shadow))
		{
			d._material->bind(
				context,
				e_st_render_pass_type::shadow,
				params,
				params->_sun_projection,
				params->_sun_view,
				d._transform);
		}

		context->draw(d);
	}

	context->end_render_pass(_pass.get());
}

void st_directional_shadow_pass::get_target_formats(st_graphics_state_desc& desc)
{
	desc._pass = _pass.get();
	desc._depth_stencil_format = _target->get_format();
}