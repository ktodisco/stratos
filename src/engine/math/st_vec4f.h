#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_vecnf.h"

/*
** Four component floating point vector.
*/
struct st_vec4f
{
	union
	{
		struct { float x, y, z, w; };
		float axes[4];
	};

	ST_VECN_FUNCTIONS(4)

	static st_vec4f zero_vector();
	static st_vec4f one_vector();
	static st_vec4f x_vector();
	static st_vec4f y_vector();
	static st_vec4f z_vector();
	static st_vec4f w_vector();
};

#undef ST_VECN_FUNCTIONS
