/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_parallax_occlusion_material.h>

#include <framework/st_frame_params.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

#include <cassert>
#include <iostream>

st_parallax_occlusion_material::st_parallax_occlusion_material(
	const char* albedo_texture,
	const char* normal_texture)
{
	st_render_context* context = st_render_context::get();
	_parallax_occlusion_buffer = context->create_buffer(1, sizeof(st_parallax_occlusion_cb), e_st_buffer_usage::uniform);
	context->add_constant(_parallax_occlusion_buffer.get(), "type_cb0", st_shader_constant_type_block);

	_albedo_texture = st_texture_loader::load(albedo_texture);
	context->set_texture_meta(_albedo_texture.get(), "SPIRV_Cross_Combineddiffuse_texturediffuse_sampler");

	_normal_texture = st_texture_loader::load(normal_texture);
	context->set_texture_meta(_normal_texture.get(), "SPIRV_Cross_Combinednormal_texturenormal_sampler");

	_resource_table = context->create_resource_table();
	st_buffer* cbs[] = { _parallax_occlusion_buffer.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
	st_texture* textures[] = { _albedo_texture.get(), _normal_texture.get() };
	context->set_textures(_resource_table.get(), std::size(textures), textures);
}

st_parallax_occlusion_material::~st_parallax_occlusion_material()
{
}

void st_parallax_occlusion_material::bind(
	class st_render_context* context,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;
	mvp.transpose();
	st_mat4f transform_t = transform;
	transform_t.transpose();

	st_parallax_occlusion_cb pom_cb{};
	pom_cb._model = transform_t;
	pom_cb._mvp = mvp;
	pom_cb._eye = st_vec4f(params->_eye, 0.0f);
	context->update_buffer(_parallax_occlusion_buffer.get(), &pom_cb, 1);

	context->bind_resource_table(_resource_table.get());
}

void st_parallax_occlusion_material::get_pipeline_state(
	struct st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_parallax_occlusion);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_blend_desc._target_blend[1]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = true;
	state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
}
