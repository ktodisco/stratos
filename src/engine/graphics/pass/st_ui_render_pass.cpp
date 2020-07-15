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
#include <gui/st_imgui.h>

st_ui_render_pass::st_ui_render_pass()
{
	_pass = std::make_unique<st_render_pass>(
		0,
		nullptr,
		nullptr);

	_vertex_format = std::make_unique<st_vertex_format>();
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_uv, 1));
	_vertex_format->finalize();

	// Set up the default UI material.
	_default_material = std::make_unique<st_constant_color_material>();

	st_pipeline_state_desc default_state_desc;
	_default_material->get_pipeline_state(&default_state_desc);

	default_state_desc._vertex_format = _vertex_format.get();
	default_state_desc._render_target_count = 1;
	default_state_desc._render_target_formats[0] = st_format_r8g8b8a8_unorm;
	default_state_desc._depth_stencil_format = st_format_d24_unorm_s8_uint;
	default_state_desc._primitive_topology_type = st_primitive_topology_type_line;

	_default_state = std::make_unique<st_pipeline_state>(default_state_desc, _pass.get());

	// Set up the font material.
	_font_material = std::make_unique<st_font_material>(nullptr);

	st_pipeline_state_desc font_state_desc;
	_font_material->get_pipeline_state(&font_state_desc);

	font_state_desc._vertex_format = _vertex_format.get();
	font_state_desc._render_target_count = 1;
	font_state_desc._render_target_formats[0] = st_format_r8g8b8a8_unorm;
	font_state_desc._depth_stencil_format = st_format_d24_unorm_s8_uint;

	_font_state = std::make_unique<st_pipeline_state>(font_state_desc, _pass.get());
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

	// Clear depth.
	context->clear(st_clear_flag_depth);

	st_imgui::draw();

	draw_dynamic(context, params, ortho, view);
}

void st_ui_render_pass::draw_dynamic(
	st_render_context* context,
	const st_frame_params* params,
	const class st_mat4f& proj,
	const class st_mat4f& view)
{
	for (auto& d : params->_gui_drawcalls)
	{
		st_render_marker draw_marker(d._name);

		if (d._material)
		{
			st_pipeline_state* state = d._material->get_material_type() == st_material_type_constant_color ?
				_default_state.get() :
				_font_state.get();
			context->set_pipeline_state(state);
			d._material->set_color(d._color);
			d._material->bind(context, params, proj, view, d._transform);
		}
		else
		{
			context->set_pipeline_state(_default_state.get());
			_default_material->set_color(d._color);
			_default_material->bind(context, params, proj, view, d._transform);
		}

		context->draw(d);
	}
}
