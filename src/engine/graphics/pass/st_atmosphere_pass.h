#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <memory>

class st_atmosphere_transmission_pass
{
public:

	st_atmosphere_transmission_pass(class st_render_texture* transmittance);
	~st_atmosphere_transmission_pass();

	void compute(class st_graphics_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<struct st_buffer> _cb;
	std::unique_ptr<struct st_buffer_view> _cbv;
	std::unique_ptr<struct st_pipeline> _pipeline;
	std::unique_ptr<struct st_texture_view> _uav;
	std::unique_ptr<struct st_resource_table> _resources;

	st_render_texture* _transmittance;
	st_texture_desc _transmittance_desc;
};

class st_atmosphere_sky_pass : public st_fullscreen_render_pass
{
public:

	st_atmosphere_sky_pass(class st_render_texture* transmittance, class st_render_texture* target);
	~st_atmosphere_sky_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<struct st_buffer> _cb;
	std::unique_ptr<struct st_buffer_view> _cbv;
	std::unique_ptr<struct st_texture_view> _sky_view;
	std::unique_ptr<struct st_pipeline> _pipeline;
	std::unique_ptr<struct st_resource_table> _resources;
	std::unique_ptr<struct st_render_pass> _pass;
	std::unique_ptr<struct st_framebuffer> _framebuffer;

	st_render_texture* _transmittance = nullptr;
	st_render_texture* _target = nullptr;
};

class st_atmosphere_render_pass : public st_fullscreen_render_pass
{
public:

	st_atmosphere_render_pass(
		class st_render_texture* target,
		class st_render_texture* depth,
		class st_render_texture* transmittance,
		class st_render_texture* sky_view);
	~st_atmosphere_render_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<struct st_buffer> _cb;
	std::unique_ptr<struct st_buffer_view> _cbv;
	std::unique_ptr<struct st_pipeline> _pipeline;
	std::unique_ptr<struct st_resource_table> _resources;
	std::unique_ptr<struct st_render_pass> _pass;
	std::unique_ptr<struct st_framebuffer> _framebuffer;

	st_render_texture* _transmittance = nullptr;
	st_render_texture* _sky_view = nullptr;
};