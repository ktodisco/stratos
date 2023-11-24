/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_constant_color_material.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_shader_manager.h>

st_constant_color_material::st_constant_color_material()
{
	st_render_context* context = st_render_context::get();
	_color_buffer = context->create_constant_buffer(sizeof(st_constant_color_cb));
	context->add_constant(_color_buffer.get(), "type_cb0", st_shader_constant_type_block);

	_resource_table = context->create_resource_table();
	st_constant_buffer* cbs[] = { _color_buffer.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
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
	context->update_constant_buffer(_color_buffer.get(), &cb_data);

	context->bind_resource_table(_resource_table.get());
}
