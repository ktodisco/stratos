/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_material.h"

#include "st_animation.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

void load_shader(const char* filename, std::string& contents)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	std::ifstream file(fullpath);

	assert(file.is_open());

	contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

bool st_material::init_shaders(const char* vs, const char* fs)
{
	std::string source_vs;
	load_shader(vs, source_vs);

	std::string source_fs;
	load_shader(fs, source_fs);

	_vs = new st_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new st_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new st_program();
	_program->attach(*_vs);
	_program->attach(*_fs);
	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	return true;
}

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

void st_unlit_texture_material::bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform)
{
	st_uniform mvp_uniform = _program->get_uniform("u_mvp");
	st_uniform texture_uniform = _program->get_uniform("u_texture");

	_program->use();

	mvp_uniform.set(transform * view * proj);
	texture_uniform.set(*_texture, 0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
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

void st_constant_color_material::bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform)
{
	st_uniform mvp_uniform = _program->get_uniform("u_mvp");
	st_uniform color_uniform = _program->get_uniform("u_color");

	_program->use();

	mvp_uniform.set(transform * view * proj);
	color_uniform.set(_color);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
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

void st_phong_material::bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform)
{
	st_uniform mvp_uniform = _program->get_uniform("u_mvp");
	st_uniform eye_uniform = _program->get_uniform("u_eye");

	_program->use();

	mvp_uniform.set(transform * view * proj);
	eye_uniform.set(view.get_translation());

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
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

void st_animated_material::bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform)
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

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
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

void st_animated_unlit_texture_material::bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform)
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

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}
