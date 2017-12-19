/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_material.h>

#include <graphics/st_animation.h>
#include <graphics/st_program.h>
#include <graphics/st_render_context.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

st_unlit_texture_material::st_unlit_texture_material(const char* texture_file) :
	_texture_file(texture_file)
{
}

st_unlit_texture_material::~st_unlit_texture_material()
{
}

bool st_unlit_texture_material::init()
{
	return st_material::init_shaders(
		"data/shaders/st_unlit_texture_vert.glsl",
		"data/shaders/st_unlit_texture_frag.glsl");
}

void st_unlit_texture_material::bind(
	st_render_context* context,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_uniform mvp_uniform = _program->get_uniform("u_mvp");
	st_uniform texture_uniform = _program->get_uniform("u_texture");

	_program->use();

	mvp_uniform.set(transform * view * proj);
	texture_uniform.set(*_texture, 0);

	context->set_blend_state(false, k_st_src_alpha, k_st_one_minus_src_alpha);
	context->set_depth_state(true, k_st_depth_less);
	context->set_depth_mask(true);
}

st_constant_color_material::st_constant_color_material()
{
}

st_constant_color_material::~st_constant_color_material()
{
}

bool st_constant_color_material::init()
{
	return st_material::init_shaders(
		"data/shaders/st_constant_color_vert.glsl",
		"data/shaders/st_constant_color_frag.glsl");
}

void st_constant_color_material::bind(
	st_render_context* context,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_uniform mvp_uniform = _program->get_uniform("u_mvp");
	st_uniform color_uniform = _program->get_uniform("u_color");

	_program->use();

	mvp_uniform.set(transform * view * proj);
	color_uniform.set(_color);

	context->set_blend_state(false, k_st_src_alpha, k_st_one_minus_src_alpha);
	context->set_depth_state(true, k_st_depth_less);
	context->set_depth_mask(true);
}

st_phong_material::st_phong_material()
{
}

st_phong_material::~st_phong_material()
{
}

bool st_phong_material::init()
{
	return st_material::init_shaders(
		"data/shaders/st_phong_vert.glsl",
		"data/shaders/st_phong_frag.glsl");
}

void st_phong_material::bind(
	st_render_context* context,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_uniform mvp_uniform = _program->get_uniform("u_mvp");
	st_uniform eye_uniform = _program->get_uniform("u_eye");

	_program->use();

	mvp_uniform.set(transform * view * proj);
	eye_uniform.set(view.get_translation());

	context->set_blend_state(false, k_st_src_alpha, k_st_one_minus_src_alpha);
	context->set_depth_state(true, k_st_depth_less);
	context->set_depth_mask(true);
}

st_animated_material::st_animated_material(st_skeleton* skeleton) : _skeleton(skeleton)
{
}

st_animated_material::~st_animated_material()
{
}

bool st_animated_material::init()
{
	return st_material::init_shaders(
		"data/shaders/st_animated_vert.glsl",
		"data/shaders/st_animated_frag.glsl");
}

void st_animated_material::bind(
	st_render_context* context,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_uniform mvp_uniform = _program->get_uniform("u_mvp");
	st_uniform skin_uniform = _program->get_uniform("u_skin");

	_program->use();

	mvp_uniform.set(transform * view * proj);
	
	// Collect the skinning matrices.
	st_mat4f skin[st_skeleton::k_max_skeleton_joints];
	for (uint32_t i = 0; i < _skeleton->_joints.size(); ++i)
	{
		assert(i < st_skeleton::k_max_skeleton_joints);
		skin[i] = _skeleton->_joints[i]->_skin;
	}
	skin_uniform.set(skin, st_skeleton::k_max_skeleton_joints);

	context->set_blend_state(false, k_st_src_alpha, k_st_one_minus_src_alpha);
	context->set_depth_state(true, k_st_depth_less);
	context->set_depth_mask(true);
}

st_animated_unlit_texture_material::st_animated_unlit_texture_material(st_skeleton* skeleton, const char* texture_file) :
	_texture_file(texture_file),
	_skeleton(skeleton)
{
}

st_animated_unlit_texture_material::~st_animated_unlit_texture_material()
{
}

bool st_animated_unlit_texture_material::init()
{
	return st_material::init_shaders(
		"data/shaders/st_animated_unlit_texture_vert.glsl",
		"data/shaders/st_animated_unlit_texture_frag.glsl");
}

void st_animated_unlit_texture_material::bind(
	st_render_context* context,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_uniform mvp_uniform = _program->get_uniform("u_mvp");
	st_uniform skin_uniform = _program->get_uniform("u_skin");
	st_uniform texture_uniform = _program->get_uniform("u_texture");

	_program->use();

	mvp_uniform.set(transform * view * proj);
	
	// Collect the skinning matrices.
	st_mat4f skin[st_skeleton::k_max_skeleton_joints];
	for (uint32_t i = 0; i < _skeleton->_joints.size(); ++i)
	{
		assert(i < st_skeleton::k_max_skeleton_joints);
		skin[i] = _skeleton->_joints[i]->_skin;
	}
	skin_uniform.set(skin, st_skeleton::k_max_skeleton_joints);

	texture_uniform.set(*_texture, 0);

	context->set_blend_state(false, k_st_src_alpha, k_st_one_minus_src_alpha);
	context->set_depth_state(true, k_st_depth_less);
	context->set_depth_mask(true);
}
