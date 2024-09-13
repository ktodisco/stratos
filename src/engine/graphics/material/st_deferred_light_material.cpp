/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_deferred_light_material.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

st_deferred_light_material::st_deferred_light_material(
	st_render_texture* albedo_texture,
	st_render_texture* normal_texture,
	st_render_texture* third_texture,
	st_render_texture* depth_texture,
	st_buffer* constants,
	st_buffer* light_buffer) :
	_albedo(albedo_texture),
	_normal(normal_texture),
	_third(third_texture),
	_depth(depth_texture)
{
	st_render_context* context = st_render_context::get();
	_resource_table = context->create_resource_table();
	context->set_constant_buffers(_resource_table.get(), 1, &constants);
	context->set_buffers(_resource_table.get(), 1, &light_buffer);

	st_texture* textures[] = {
		_albedo->get_texture(),
		_normal->get_texture(),
		_third->get_texture(),
		_depth->get_texture(),
	};
	context->set_textures(_resource_table.get(), std::size(textures), textures);
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
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_texture_meta(_albedo->get_texture(), "SPIRV_Cross_Combinedalbedo_textureSPIRV_Cross_DummySampler");
	context->set_texture_meta(_normal->get_texture(), "SPIRV_Cross_Combinednormal_textureSPIRV_Cross_DummySampler");
	context->set_texture_meta(_third->get_texture(), "SPIRV_Cross_Combinedthird_textureSPIRV_Cross_DummySampler");
	context->set_texture_meta(_depth->get_texture(), "SPIRV_Cross_Combineddepth_textureSPIRV_Cross_DummySampler");

	context->transition(_albedo->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_normal->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_third->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_depth->get_texture(), st_texture_state_pixel_shader_read);

	context->bind_resource_table(_resource_table.get());
}
