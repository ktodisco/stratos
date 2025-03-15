#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <memory>

class st_smaa_pass : public st_fullscreen_render_pass
{
public:
	st_smaa_pass(
		class st_render_texture* source_buffer,
		class st_render_texture* stencil_buffer,
		struct st_swap_chain* swap_chain);
	~st_smaa_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	void _create_edges_pass(class st_graphics_context* context);
	void _create_weights_pass(class st_graphics_context* context);
	void _create_blend_pass(class st_graphics_context* context, struct st_swap_chain* swap_chain);

	void _render_edges_pass(class st_graphics_context* context, const struct st_frame_params* params);
	void _render_weights_pass(class st_graphics_context* context, const struct st_frame_params* params);
	void _render_blend_pass(class st_graphics_context* context, const struct st_frame_params* params);

private:
	st_render_texture* _source_buffer = nullptr;
	st_render_texture* _stencil_buffer = nullptr;

	std::unique_ptr<struct st_buffer> _cb = nullptr;
	std::unique_ptr<struct st_buffer_view> _cbv = nullptr;

	std::unique_ptr<class st_render_texture> _edges_target = nullptr;
	std::unique_ptr<struct st_render_pass> _edges_pass = nullptr;
	std::unique_ptr<struct st_framebuffer> _edges_framebuffer = nullptr;
	std::unique_ptr<struct st_pipeline> _edges_pipeline = nullptr;
	std::unique_ptr<struct st_resource_table> _edges_resources = nullptr;

	std::unique_ptr<struct st_texture> _area_tex = nullptr;
	std::unique_ptr<struct st_texture> _search_tex = nullptr;
	std::unique_ptr<struct st_texture_view> _area_tex_view = nullptr;
	std::unique_ptr<struct st_texture_view> _search_tex_view = nullptr;

	std::unique_ptr<class st_render_texture> _weights_target = nullptr;
	std::unique_ptr<struct st_render_pass> _weights_pass = nullptr;
	std::unique_ptr<struct st_framebuffer> _weights_framebuffer = nullptr;
	std::unique_ptr<struct st_pipeline> _weights_pipeline = nullptr;
	std::unique_ptr<struct st_resource_table> _weights_resources = nullptr;

	std::unique_ptr<struct st_render_pass> _blend_pass = nullptr;
	std::unique_ptr<struct st_framebuffer> _blend_framebuffers[k_max_frames];
	std::unique_ptr<struct st_pipeline> _blend_pipeline = nullptr;
	std::unique_ptr<struct st_resource_table> _blend_resources = nullptr;
};
