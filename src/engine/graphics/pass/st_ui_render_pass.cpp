/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_ui_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/material/st_constant_color_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

#include <gui/st_font.h>
#include <gui/st_imgui.h>

st_ui_render_pass::st_ui_render_pass(const st_window* window, st_render_texture* target)
{
	st_graphics_context* context = st_graphics_context::get();

	_target_format = target->get_format();

	{
		st_attachment_desc attachment = { _target_format, e_st_load_op::load, e_st_store_op::store };
		st_render_pass_desc desc;
		desc._attachments = &attachment;
		desc._attachment_count = 1;
		desc._viewport = { 0.0f, 0.0f, float(target->get_width()), float(target->get_height()), 0.0f, 1.0f };

		_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc target_desc = { target->get_texture(), target->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _pass.get();
		desc._targets = &target_desc;
		desc._target_count = 1;

		_framebuffer = context->create_framebuffer(desc);
	}

	std::vector<st_vertex_attribute> attributes;
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_position, st_format_r32g32b32_float, 0));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_color, st_format_r32g32b32_float, 1));
	_vertex_format = context->create_vertex_format(attributes.data(), attributes.size());

	// Set up the default UI material.
	_default_material = std::make_unique<st_constant_color_material>();

	st_graphics_state_desc default_state_desc;
	default_state_desc._shader = st_shader_manager::get()->get_shader(st_shader_constant_color);
	default_state_desc._blend_desc._target_blend[0]._blend = false;
	default_state_desc._depth_stencil_desc._depth_enable = false;
	default_state_desc._depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
	default_state_desc._vertex_format = _vertex_format.get();
	default_state_desc._pass = _pass.get();
	default_state_desc._render_target_count = 1;
	default_state_desc._render_target_formats[0] = _target_format;
	default_state_desc._primitive_topology_type = st_primitive_topology_type_line;

	_default_state = context->create_graphics_pipeline(default_state_desc);

	st_imgui::initialize(window, context, _target_format, _pass.get());
}

st_ui_render_pass::~st_ui_render_pass()
{
	st_imgui::shutdown();
}

void st_ui_render_pass::render(st_graphics_context* context, const st_frame_params* params)
{
	st_render_marker marker(context, "st_ui_render_pass::render");

	st_mat4f ortho;
	ortho.make_orthographic(0.0f, (float)params->_width, (float)params->_height, 0.0f, 0.1f, 10000.0f);
	st_mat4f view;
	view.make_lookat_rh(st_vec3f::z_vector(), -st_vec3f::z_vector(), st_vec3f::y_vector());

	context->begin_render_pass(_pass.get(), _framebuffer.get(), nullptr, 0);

	draw_dynamic(context, params, ortho, view);
	st_imgui::draw(params);

	context->end_render_pass(_pass.get(), _framebuffer.get());
}

void st_ui_render_pass::draw_dynamic(
	st_graphics_context* context,
	const st_frame_params* params,
	const struct st_mat4f& proj,
	const struct st_mat4f& view)
{
	context->set_scissor(0, 0, params->_width, params->_height);

	for (auto& d : params->_gui_drawcalls)
	{
		st_render_marker draw_marker(context, d._name);

		context->set_pipeline(_default_state.get());
		_default_material->set_color(d._color);
		_default_material->bind(context, e_st_render_pass_type::ui, params, proj, view, d._transform);

		context->draw(d);
	}
}

void st_ui_render_pass::get_target_formats(struct st_graphics_state_desc& desc)
{
	desc._pass = _pass.get();
	desc._render_target_count = 1;
	desc._render_target_formats[0] = _target_format;
}
