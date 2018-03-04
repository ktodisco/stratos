#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <memory>

class st_fullscreen_render_pass
{
public:
	// TODO: This should also take the 'backbuffer' render target.
	st_fullscreen_render_pass(
		class st_render_texture* input);
	~st_fullscreen_render_pass();

	void render(class st_render_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<class st_vertex_format> _vertex_format = nullptr;
	std::unique_ptr<class st_geometry> _fullscreen_quad = nullptr;

	std::unique_ptr<class st_fullscreen_material> _default_material = nullptr;
	std::unique_ptr<class st_pipeline_state> _pipeline_state = nullptr;
};
