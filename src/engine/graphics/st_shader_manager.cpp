/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_shader_manager.h>

#include <graphics/st_shader.h>

st_shader_manager* st_shader_manager::_this = nullptr;

st_shader_manager::st_shader_manager()
{
	_shaders[st_shader_phong] =
		std::make_unique<st_shader>(
			"data/shaders/st_phong",
			st_shader_type_vertex | st_shader_type_pixel);
	_shaders[st_shader_unlit_texture] =
		std::make_unique<st_shader>(
			"data/shaders/st_unlit_texture",
			st_shader_type_vertex | st_shader_type_pixel);
	_shaders[st_shader_constant_color] =
		std::make_unique<st_shader>(
			"data/shaders/st_constant_color",
			st_shader_type_vertex | st_shader_type_pixel);
	_shaders[st_shader_font] =
		std::make_unique<st_shader>(
			"data/shaders/st_font_simple",
			st_shader_type_vertex | st_shader_type_pixel);
	_shaders[st_shader_gbuffer] =
		std::make_unique<st_shader>(
			"data/shaders/st_gbuffer",
			st_shader_type_vertex | st_shader_type_pixel);
	_shaders[st_shader_fullscreen] =
		std::make_unique<st_shader>(
			"data/shaders/st_fullscreen",
			st_shader_type_vertex | st_shader_type_pixel);
	_shaders[st_shader_deferred_light] =
		std::make_unique<st_shader>(
			"data/shaders/st_deferred_light",
			st_shader_type_vertex | st_shader_type_pixel);

	_this = this;
}

st_shader_manager::~st_shader_manager()
{
	_shaders.clear();
}

const st_shader* st_shader_manager::get_shader(e_st_shader shader)
{
	return _shaders[shader].get();
}

st_shader_manager* st_shader_manager::get()
{
	return _this;
}
