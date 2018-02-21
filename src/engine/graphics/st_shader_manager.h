#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <map>
#include <memory>

enum e_st_shader_type_flag
{
	st_shader_type_vertex = 1,
	st_shader_type_pixel = 2,
	st_shader_type_domain = 4,
	st_shader_type_hull = 8,
	st_shader_type_geometry = 16,
	st_shader_type_compute = 32,
};

enum e_st_shader
{
	st_shader_phong,
	st_shader_unlit_texture,
	st_shader_constant_color,
	st_shader_font,
};

class st_shader_manager
{
public:
	st_shader_manager();
	~st_shader_manager();

	const class st_shader* get_shader(e_st_shader shader);

	static st_shader_manager* get();

private:
	std::map<e_st_shader, std::unique_ptr<class st_shader>> _shaders;

	static st_shader_manager* _this;
};
