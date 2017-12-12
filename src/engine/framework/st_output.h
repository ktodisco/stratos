#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_drawcall.h"
#include "math/st_mat4f.h"

#include <vector>

class st_window;
class st_render_context;

/*
** Represents the output stage of the frame.
** Owns whatever is drawn on the screen.
*/
class st_output
{
public:

	st_output(const st_window* window, st_render_context* render_context);
	~st_output();

	void update(struct st_frame_params* params);

private:

	const st_window* _window;
	st_render_context* _render_context;
};
