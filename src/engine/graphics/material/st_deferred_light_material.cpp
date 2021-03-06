/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_deferred_light_material.h>

#include <graphics/st_constant_buffer.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_resource_table.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture.h>

st_deferred_light_material::st_deferred_light_material(
	st_texture* albedo_texture,
	st_texture* normal_texture,
	st_texture* third_texture,
	st_texture* depth_texture,
	st_constant_buffer* constants,
	st_buffer* light_buffer) :
	_albedo(albedo_texture),
	_normal(normal_texture),
	_third(third_texture),
	_depth(depth_texture)
{
	_resource_table = std::make_unique<st_resource_table>();
	_resource_table->set_constant_buffers(1, &constants);
	_resource_table->set_buffers(1, &light_buffer);

	st_texture* textures[] = {
		_albedo,
		_normal,
		_third,
		_depth,
	};
	_resource_table->set_textures(std::size(textures), textures);
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
	_albedo->set_meta("SPIRV_Cross_Combinedalbedo_textureSPIRV_Cross_DummySampler");
	_normal->set_meta("SPIRV_Cross_Combinednormal_textureSPIRV_Cross_DummySampler");
	_third->set_meta("SPIRV_Cross_Combinedthird_textureSPIRV_Cross_DummySampler");
	_depth->set_meta("SPIRV_Cross_Combineddepth_textureSPIRV_Cross_DummySampler");

	_albedo->transition(context, st_texture_state_pixel_shader_read);
	_normal->transition(context, st_texture_state_pixel_shader_read);
	_third->transition(context, st_texture_state_pixel_shader_read);
	_depth->transition(context, st_texture_state_pixel_shader_read);

	_resource_table->bind(context);
}
