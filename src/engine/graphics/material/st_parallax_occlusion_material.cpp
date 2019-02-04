/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_parallax_occlusion_material.h>

#include <framework/st_frame_params.h>
#include <graphics/st_constant_buffer.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_resource_table.h>
#include <graphics/st_shader_manager.h>

#include <cassert>
#include <iostream>

st_parallax_occlusion_material::st_parallax_occlusion_material(
	const char* albedo_texture,
	const char* normal_texture)
{
	_parallax_occlusion_buffer = std::make_unique<st_constant_buffer>(sizeof(st_parallax_occlusion_cb));
	_parallax_occlusion_buffer->add_constant("type_cb0", st_shader_constant_type_block);

	_albedo_texture = std::make_unique<st_texture>();
	if (!_albedo_texture->load_from_file(albedo_texture))
	{
		std::cerr << "Failed to load " << albedo_texture << std::endl;
		assert(false);
	}
	_albedo_texture->set_meta("SPIRV_Cross_Combineddiffuse_texturediffuse_sampler");

	_normal_texture = std::make_unique<st_texture>();
	if (!_normal_texture->load_from_file(normal_texture))
	{
		std::cerr << "Failed to load " << normal_texture << std::endl;
		assert(false);
	}
	_normal_texture->set_meta("SPIRV_Cross_Combinednormal_texturenormal_sampler");

	_resource_table = std::make_unique<st_resource_table>();
	_resource_table->add_constant_buffer(_parallax_occlusion_buffer.get());
	_resource_table->add_texture_resource(_albedo_texture.get());
	_resource_table->add_texture_resource(_normal_texture.get());
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
	_parallax_occlusion_buffer->update(context, &pom_cb);

	_resource_table->bind(context);
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
