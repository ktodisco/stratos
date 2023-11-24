/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_phong_material.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_shader_manager.h>

st_phong_material::st_phong_material()
{
	st_render_context* context = st_render_context::get();
	_phong_buffer = context->create_constant_buffer(sizeof(st_view_cb));
	context->add_constant(_phong_buffer.get(), "type_cb0", st_shader_constant_type_block);

	_resource_table = context->create_resource_table();
	st_constant_buffer* cbs[] = { _phong_buffer.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
}

st_phong_material::~st_phong_material()
{
}

void st_phong_material::bind(
	st_render_context* context,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;
	mvp.transpose();

	st_view_cb cb_data{};
	cb_data._mvp = mvp;
	context->update_constant_buffer(_phong_buffer.get(), &cb_data);

	context->bind_resource_table(_resource_table.get());
}

void st_phong_material::get_pipeline_state(
	st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_phong);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = true;
	state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
}
