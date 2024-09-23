#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <memory>
#include <vector>

class st_gaussian_blur_render_pass : public st_fullscreen_render_pass
{
public:
	st_gaussian_blur_render_pass(
		class st_render_texture* source_buffer,
		class st_render_texture* target_buffer);
	~st_gaussian_blur_render_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<class st_render_pass> _vertical_blur_pass = nullptr;
	std::unique_ptr<class st_render_pass> _horizontal_blur_pass = nullptr;

	std::unique_ptr<class st_render_texture> _intermediate_target = nullptr;

	// Note: It's wasteful on memory to not reuse the base fullscreen pass material, but
	// this is more verbose and clear.  This could be resolved if the fullscreen quad is
	// abstracted out of a base class.
	std::unique_ptr<class st_material> _vertical_blur_material = nullptr;
	std::unique_ptr<class st_material> _horizontal_blur_material = nullptr;

	std::unique_ptr<class st_pipeline> _vertical_blur_state = nullptr;
	std::unique_ptr<class st_pipeline> _horizontal_blur_state = nullptr;
};
