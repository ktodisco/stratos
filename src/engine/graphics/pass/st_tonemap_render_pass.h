#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <memory>

class st_tonemap_render_pass : public st_fullscreen_render_pass
{
public:
	st_tonemap_render_pass(
		class st_render_texture* source_buffer,
		class st_render_texture* bloom_buffer,
		class st_render_texture* target_buffer);
	~st_tonemap_render_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<struct st_buffer> _cb = nullptr;
	std::unique_ptr<struct st_buffer_view> _cbv = nullptr;
	std::unique_ptr<struct st_render_pass> _pass = nullptr;
	std::unique_ptr<struct st_framebuffer> _framebuffer = nullptr;
	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;
	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;

	class st_render_texture* _source;
	class st_render_texture* _bloom;
};
