/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_ui_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_vertex_format.h>
#include <graphics/material/st_constant_color_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>

#include <gui/st_font.h>

st_ui_render_pass::st_ui_render_pass()
{
	_vertex_format = std::make_unique<st_vertex_format>();
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_uv, 3));
	_vertex_format->finalize();

	// Set up the default UI material.
	_default_material = std::make_unique<st_constant_color_material>();

	st_pipeline_state_desc default_state_desc;
	_default_material->get_pipeline_state(&default_state_desc);

	default_state_desc._vertex_format = _vertex_format.get();
	default_state_desc._render_target_count = 1;
	default_state_desc._render_target_formats[0] = st_texture_format_r8g8b8a8_unorm;
	default_state_desc._depth_stencil_format = st_texture_format_d24_unorm_s8_uint;
	default_state_desc._primitive_topology_type = st_primitive_topology_type_line;

	_default_state = std::make_unique<st_pipeline_state>(default_state_desc);

	// Set up the font material.
	_font_material = std::make_unique<st_font_material>(nullptr);

	st_pipeline_state_desc font_state_desc;
	_font_material->get_pipeline_state(&font_state_desc);

	font_state_desc._vertex_format = _vertex_format.get();
	font_state_desc._render_target_count = 1;
	font_state_desc._render_target_formats[0] = st_texture_format_r8g8b8a8_unorm;
	font_state_desc._depth_stencil_format = st_texture_format_d24_unorm_s8_uint;

	_font_state = std::make_unique<st_pipeline_state>(font_state_desc);
}

st_ui_render_pass::~st_ui_render_pass()
{
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
			context->set_pipeline_state(_font_state.get());
			d._material->set_color(d._color);
			d._material->bind(context, proj, view, d._transform);
		}
		else
		{
			context->set_pipeline_state(_default_state.get());
			_default_material->set_color(d._color);
			_default_material->bind(context, proj, view, d._transform);
		}

		context->draw(d);
	}
}