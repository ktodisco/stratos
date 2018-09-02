/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_constant_color_material.h>

#include <graphics/st_constant_buffer.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_resource_table.h>
#include <graphics/st_shader_manager.h>

st_constant_color_material::st_constant_color_material()
{
	_color_buffer = std::make_unique<st_constant_buffer>(sizeof(st_constant_color_cb));
	_color_buffer->add_constant("u_mvp", st_shader_constant_type_mat4);
	_color_buffer->add_constant("u_color", st_shader_constant_type_vec3);

	_resource_table = std::make_unique<st_resource_table>();
	_resource_table->add_constant_buffer(_color_buffer.get());
}

st_constant_color_material::~st_constant_color_material()
{
}

void st_constant_color_material::get_pipeline_state(
	struct st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_constant_color);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = true;
	state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
}

void st_constant_color_material::bind(
	st_render_context* context,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;
	mvp.transpose();

	st_constant_color_cb cb_data{};
	cb_data._mvp = mvp;
	cb_data._color = _color;
	_color_buffer->update(context, &cb_data);

	_resource_table->bind(context);
}
