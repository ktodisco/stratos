#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_fullscreen_render_pass.h>

class st_passthrough_render_pass : public st_fullscreen_render_pass
{
public:
	st_passthrough_render_pass(
		class st_render_texture* source_buffer);
	~st_passthrough_render_pass();

	void render(class st_render_context* context, const struct st_frame_params* params);
};
