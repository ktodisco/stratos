#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include "math/st_mat4f.h"
#include "math/st_vec3f.h"

#include <string>

#include <Windows.h>
#include <wrl.h>

/*
** Base class for all graphical materials.
** Includes the shaders and other state necessary to draw geometry.
*/
class st_dx12_material
{
public:
	virtual void bind(
		class st_render_context* context,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) = 0;

	virtual void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) = 0;
};
