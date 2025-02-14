/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_shader_manager.h>

#include <graphics/st_graphics_context.h>

st_shader_manager* st_shader_manager::_this = nullptr;

st_shader_manager::st_shader_manager(st_graphics_context* context)
{
	_shaders[st_shader_phong] =
		context->create_shader(
			"data/shaders/st_phong",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_unlit_texture] =
		context->create_shader(
			"data/shaders/st_unlit_texture",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_constant_color] =
		context->create_shader(
			"data/shaders/st_constant_color",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_font] =
		context->create_shader(
			"data/shaders/st_font_simple",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_gbuffer] =
		context->create_shader(
			"data/shaders/st_gbuffer",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_fullscreen] =
		context->create_shader(
			"data/shaders/st_fullscreen",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_deferred_light] =
		context->create_shader(
			"data/shaders/st_deferred_light",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_tonemap] =
		context->create_shader(
			"data/shaders/st_tonemap",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_bloom_threshold] =
		context->create_shader(
			"data/shaders/st_bloom_threshold",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_bloom_downsample] =
		context->create_shader(
			"data/shaders/st_bloom_downsample",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_bloom_upsample] =
		context->create_shader(
			"data/shaders/st_bloom_upsample",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_gaussian_blur_vertical] =
		context->create_shader(
			"data/shaders/st_gaussian_blur_vertical",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_gaussian_blur_horizontal] =
		context->create_shader(
			"data/shaders/st_gaussian_blur_horizontal",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_parallax_occlusion] =
		context->create_shader(
			"data/shaders/st_parallax_occlusion",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_imgui] =
		context->create_shader(
			"data/shaders/imgui",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_shadow] =
		context->create_shader(
			"data/shaders/st_shadow",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_atmosphere_transmission] =
		context->create_shader(
			"data/shaders/st_atmosphere_transmission",
			e_st_shader_type::compute);
	_shaders[st_shader_atmosphere_sky_view] =
		context->create_shader(
			"data/shaders/st_atmosphere_sky_view",
			e_st_shader_type::vertex | e_st_shader_type::pixel);
	_shaders[st_shader_atmosphere] =
		context->create_shader(
			"data/shaders/st_atmosphere_apply",
			e_st_shader_type::vertex | e_st_shader_type::pixel);

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
