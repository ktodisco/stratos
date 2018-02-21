#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_mat4f.h"
#include "math/st_vec3f.h"

#include <string>

/*
** Base class for all graphical materials.
** Includes the shaders and other state necessary to draw geometry.
*/
class st_gl_material
{
public:
	virtual bool init() = 0;

	virtual void bind(
		class st_render_context* context,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) = 0;

protected:
	bool init_shaders(const char* shader_name);

protected:
	class st_gl_shader* _vs;
	class st_gl_shader* _fs;
	class st_gl_program* _program;
};
