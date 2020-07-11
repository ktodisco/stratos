#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <memory>
#include <vector>

class st_bloom_render_pass : public st_fullscreen_render_pass
{
public:
	st_bloom_render_pass(
		class st_render_texture* source_buffer,
		class st_render_texture* target_buffer);
	~st_bloom_render_pass();

	void render(class st_render_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<class st_render_pass> _pass = nullptr;

	std::vector<std::unique_ptr<class st_render_texture>> _targets;

	std::unique_ptr<class st_gaussian_blur_render_pass> _blur_pass = nullptr;
};
