/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_phong_material.h>

#include <graphics/st_constant_buffer.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_resource_table.h>
#include <graphics/st_shader_manager.h>

st_phong_material::st_phong_material()
{
	_phong_buffer = std::make_unique<st_constant_buffer>(sizeof(st_view_cb));
	_phong_buffer->add_constant("u_mvp", st_shader_constant_type_mat4);

	_resource_table = std::make_unique<st_resource_table>();
	_resource_table->add_constant_buffer(_phong_buffer.get());
}

st_phong_material::~st_phong_material()
{
}

void st_phong_material::bind(
	st_render_context* context,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;
	mvp.transpose();

	st_view_cb cb_data{};
	cb_data._mvp = mvp;
	_phong_buffer->update(context, &cb_data);

	_resource_table->bind(context);
}

void st_phong_material::get_pipeline_state(
	st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_phong);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = true;
	state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
}
