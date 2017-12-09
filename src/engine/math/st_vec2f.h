#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_vecnf.h"

/*
** Two component floating point vector.
*/
struct st_vec2f
{
	union
	{
		struct { float x, y; };
		float axes[2];
	};

	st_VECN_FUNCTIONS(2)

	static st_vec2f zero_vector();
	static st_vec2f one_vector();
	static st_vec2f x_vector();
	static st_vec2f y_vector();
};

#undef st_VECN_FUNCTIONS
