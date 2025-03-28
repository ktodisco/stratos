#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "graphics/st_drawcall.h"
#include "math/st_mat4f.h"

#include <memory>
#include <vector>

class st_window;

enum class e_st_render_pass_type : uint64_t
{
	gbuffer			= (1 << 0),
	deferred		= (1 << 1),
	bloom			= (1 << 2),
	gaussian		= (1 << 3),
	tonemap			= (1 << 4),
	passthrough		= (1 << 5),
	ui				= (1 << 6),
	shadow			= (1 << 7),
};
using e_st_render_pass_type_flags = st_flags<e_st_render_pass_type, uint64_t>;
ST_ENUM_FLAG_OPS(e_st_render_pass_type, e_st_render_pass_type_flags);

/*
** Represents the output stage of the frame.
** Owns whatever is drawn on the screen.
*/
class st_output
{
public:

	st_output(const st_window* window, class st_graphics_context* graphics);
	~st_output();

	bool update_swap_chain();
	void update(struct st_frame_params* params);

	void get_target_formats(e_st_render_pass_type type, struct st_graphics_state_desc& desc);

	static st_output* get() { return _this; }

private:

	e_st_format choose_backbuffer_format();

	void recreate_textures(class st_graphics_context* context);
	void recreate_passes(class st_graphics_context* context);

	const st_window* _window;
	uint32_t _width;
	uint32_t _height;
	e_st_format _backbuffer_format;
	e_st_color_space _backbuffer_color_space;
	class st_graphics_context* _graphics_context;

	std::unique_ptr<struct st_swap_chain> _swap_chain;
	bool _out_of_date = false;

	// Atmospherics.
	std::unique_ptr<class st_render_texture> _transmittance;
	std::unique_ptr<class st_render_texture> _sky_view;
	std::unique_ptr<class st_atmosphere_transmission_pass> _atmosphere_transmission;
	std::unique_ptr<class st_atmosphere_sky_pass> _atmosphere_sky;
	std::unique_ptr<class st_atmosphere_render_pass> _atmosphere_pass;

	std::unique_ptr<class st_directional_shadow_pass> _directional_shadow_pass;
	std::unique_ptr<class st_gbuffer_render_pass> _gbuffer_pass;
	std::unique_ptr<class st_deferred_light_render_pass> _deferred_pass;
	std::unique_ptr<class st_bloom_render_pass> _bloom_pass;
	std::unique_ptr<class st_tonemap_render_pass> _tonemap_pass;
	std::unique_ptr<class st_smaa_pass> _smaa_pass;
	std::unique_ptr<class st_ui_render_pass> _ui_pass;
	std::unique_ptr<class st_display_pass> _display_pass;

	std::unique_ptr<class st_render_texture> _directional_shadow_map;

	std::unique_ptr<class st_render_texture> _gbuffer_albedo_target;
	std::unique_ptr<class st_render_texture> _gbuffer_normal_target;
	std::unique_ptr<class st_render_texture> _gbuffer_third_target;
	std::unique_ptr<class st_render_texture> _depth_stencil_target;

	std::unique_ptr<class st_render_texture> _deferred_target;
	std::unique_ptr<class st_render_texture> _bloom_target;
	std::unique_ptr<class st_render_texture> _tonemap_target;

	static st_output* _this;
};
