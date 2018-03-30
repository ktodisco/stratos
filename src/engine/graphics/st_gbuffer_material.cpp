/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_gbuffer_material.h>

#include <graphics/st_constant_buffer.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_resource_table.h>
#include <graphics/st_shader_manager.h>

#include <cassert>
#include <iostream>

st_gbuffer_material::st_gbuffer_material(const char* texture_file) :
	_texture_file(texture_file)
{
	_view_buffer = std::make_unique<st_constant_buffer>(sizeof(st_view_cb));
	_view_buffer->add_constant("u_mvp", st_shader_constant_type_mat4);

	_texture = std::make_unique<st_texture>();
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load " << _texture_file << std::endl;
		assert(false);
	}
	_texture->set_meta("u_texture", 0);

	_resource_table = std::make_unique<st_resource_table>();
	_resource_table->add_constant_buffer(_view_buffer.get());
	_resource_table->add_shader_resource(_texture.get());
}

st_gbuffer_material::~st_gbuffer_material()
{
}

void st_gbuffer_material::get_pipeline_state(
	st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_gbuffer);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_blend_desc._target_blend[1]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = true;
	state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
}

void st_gbuffer_material::bind(
	st_render_context* context,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;
	mvp.transpose();

	st_view_cb cb_data{};
	cb_data._mvp = mvp;
	_view_buffer->update(context, &cb_data);

	_resource_table->bind(context);
}
