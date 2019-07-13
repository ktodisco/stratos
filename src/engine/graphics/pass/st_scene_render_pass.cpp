/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_scene_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_vertex_format.h>
#include <graphics/material/st_material.h>
#include <graphics/material/st_phong_material.h>
#include <graphics/material/st_unlit_texture_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>

st_scene_render_pass::st_scene_render_pass()
{
	_vertex_format = std::make_unique<st_vertex_format>();
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_normal, 1));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_tangent, 2));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_color, 3));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_uv, 4));
	_vertex_format->finalize();

	// Set up the phong material and state.
	_default_phong = std::make_unique<st_phong_material>();

	st_pipeline_state_desc phong_state_desc;
	_default_phong->get_pipeline_state(&phong_state_desc);

	phong_state_desc._vertex_format = _vertex_format.get();
	phong_state_desc._render_target_count = 1;
	phong_state_desc._render_target_formats[0] = st_format_r8g8b8a8_unorm;
	phong_state_desc._depth_stencil_format = st_format_d24_unorm_s8_uint;

	_phong_state = std::make_unique<st_pipeline_state>(phong_state_desc);

	// Set up the unlit texture material and state.
	_default_unlit_texture = std::make_unique<st_unlit_texture_material>("data/textures/test.png");

	st_pipeline_state_desc unlit_texture_state_desc;
	_default_unlit_texture->get_pipeline_state(&unlit_texture_state_desc);

	unlit_texture_state_desc._vertex_format = _vertex_format.get();
	unlit_texture_state_desc._render_target_count = 1;
	unlit_texture_state_desc._render_target_formats[0] = st_format_r8g8b8a8_unorm;
	unlit_texture_state_desc._depth_stencil_format = st_format_d24_unorm_s8_uint;

	_unlit_texture_state = std::make_unique<st_pipeline_state>(unlit_texture_state_desc);
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

	context->set_pipeline_state(_phong_state.get());

	// Draw all static geometry:
	for (auto& d : params->_static_drawcalls)
	{
		st_render_marker draw_marker(d._name.c_str());

		if (d._material->get_material_type() == st_material_type_phong)
		{
			context->set_pipeline_state(_phong_state.get());
		}
		else if (d._material->get_material_type() == st_material_type_unlit_texture)
		{
			context->set_pipeline_state(_unlit_texture_state.get());
		}

		d._material->bind(context, params, perspective, params->_view, d._transform);
		context->draw(d);
	}

	// Draw all dynamic geometry:
	draw_dynamic(context, params, perspective, params->_view);
}

void st_scene_render_pass::draw_dynamic(
	st_render_context* context,
	const st_frame_params* params,
	const class st_mat4f& proj,
	const class st_mat4f& view)
{
	for (auto& d : params->_dynamic_drawcalls)
	{
		st_render_marker draw_marker(d._name);

		if (d._material)
		{
			//d._material->set_color(d._color);
			d._material->bind(context, params, proj, view, d._transform);
		}
		else
		{
			//_default_material->set_color(d._color);
			//_default_material->bind(context, proj, view, d._transform);
		}

		context->draw(d);
	}
}
