/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_tonemap_material.h>

#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_resource_table.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture.h>

st_tonemap_material::st_tonemap_material(st_texture* texture) :
	_texture(texture)
{
	_resource_table = std::make_unique<st_resource_table>();
	_resource_table->add_texture_resource(_texture);
}

st_tonemap_material::~st_tonemap_material()
{
}

void st_tonemap_material::get_pipeline_state(
	st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_tonemap);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = false;
}

void st_tonemap_material::bind(
	st_render_context* context,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	_texture->set_meta("SPIRV_Cross_Combinedtextex_sampler");
	_resource_table->bind(context);
}
