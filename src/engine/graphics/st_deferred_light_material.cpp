/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_deferred_light_material.h>

#include <graphics/st_constant_buffer.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_resource_table.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture.h>

st_deferred_light_material::st_deferred_light_material(
	st_texture* albedo_texture,
	st_texture* normal_texture,
	st_texture* depth_texture,
	st_constant_buffer* light_buffer) :
	_albedo(albedo_texture),
	_normal(normal_texture),
	_depth(depth_texture)
{
	_resource_table = std::make_unique<st_resource_table>();
	_resource_table->add_constant_buffer(light_buffer);
	_resource_table->add_shader_resource(_albedo);
	_resource_table->add_shader_resource(_normal);
	// TODO: The d24_s8 format cannot be used with an SRV.
	_resource_table->add_shader_resource(_depth);
}

st_deferred_light_material::~st_deferred_light_material()
{
}

void st_deferred_light_material::get_pipeline_state(
	st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_deferred_light);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = false;
}

void st_deferred_light_material::bind(
	st_render_context* context,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	_albedo->set_meta("u_albedo", 0);
	_normal->set_meta("u_normal", 1);
	_depth->set_meta("u_depth", 2);

	_resource_table->bind(context);
}
