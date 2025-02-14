#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <core/st_flags.h>

#include <map>
#include <memory>

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
	st_shader_bloom_threshold,
	st_shader_bloom_downsample,
	st_shader_bloom_upsample,
	st_shader_gaussian_blur_vertical,
	st_shader_gaussian_blur_horizontal,
	st_shader_parallax_occlusion,
	st_shader_shadow,
	st_shader_atmosphere_transmission,
	st_shader_atmosphere_sky_view,
	st_shader_atmosphere,

	st_shader_imgui,
};

class st_shader_manager
{
public:
	st_shader_manager(class st_graphics_context* context);
	~st_shader_manager();

	const struct st_shader* get_shader(e_st_shader shader);

	static st_shader_manager* get();

private:
	std::map<e_st_shader, std::unique_ptr<struct st_shader>> _shaders;

	static st_shader_manager* _this;
};
