/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_material.h>

#include <graphics/st_animation.h>
#include <graphics/st_constant_buffer.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_vertex_format.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

st_unlit_texture_material::st_unlit_texture_material(const char* texture_file) :
	_texture_file(texture_file)
{
	_view_buffer = std::make_unique<st_constant_buffer>(sizeof(st_view_cb));

	_texture = std::make_unique<st_texture>();
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load " << _texture_file << std::endl;
		assert(false);
	}
}

st_unlit_texture_material::~st_unlit_texture_material()
{
}

void st_unlit_texture_material::get_pipeline_state(
	struct st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_unlit_texture);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = true;
	state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
}

void st_unlit_texture_material::bind(
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

	_view_buffer->commit(context);

	_texture->bind(context);
}

st_constant_color_material::st_constant_color_material()
{
	_color_buffer = std::make_unique<st_constant_buffer>(sizeof(st_constant_color_cb));
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
	_color_buffer->commit(context);
}

st_phong_material::st_phong_material()
{
	_phong_buffer = std::make_unique<st_constant_buffer>(sizeof(st_view_cb));
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

	_phong_buffer->commit(context);
}

void st_phong_material::get_pipeline_state(
	st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_phong);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = true;
	state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
}
