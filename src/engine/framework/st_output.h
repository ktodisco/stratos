#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_drawcall.h"
#include "math/st_mat4f.h"

#include <vector>

/*
** Represents the output stage of the frame.
** Owns whatever is drawn on the screen.
*/
class st_output
{
public:
	st_output(void* win);
	~st_output();

	void update(struct st_frame_params* params);

private:
	void* _window;
};
