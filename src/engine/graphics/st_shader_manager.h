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
	st_shader_gbuffer,
	st_shader_fullscreen,
	st_shader_deferred_light,
	st_shader_tonemap,
	st_shader_bloom,
	st_shader_gaussian_blur_vertical,
	st_shader_gaussian_blur_horizontal,
	st_shader_parallax_occlusion,
};

class st_shader_manager
{
public:
	st_shader_manager(class st_render_context* context);
	~st_shader_manager();

	const struct st_shader* get_shader(e_st_shader shader);

	static st_shader_manager* get();

private:
	std::map<e_st_shader, std::unique_ptr<struct st_shader>> _shaders;

	static st_shader_manager* _this;
};
