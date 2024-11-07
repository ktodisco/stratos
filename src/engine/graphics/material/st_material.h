#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_output.h>

#include <math/st_mat4f.h>
#include <math/st_vec3f.h>

#include <memory>
#include <string>

#include <wrl.h>

/*
** Base class for all graphical materials.
** Includes the shaders and other state necessary to draw geometry.
*/
class st_material
{
public:
	st_material(e_st_render_pass_type_flags passes) : _supported_passes(passes) {}
	virtual ~st_material() {}
	virtual void bind(
		class st_graphics_context* context,
		enum e_st_render_pass_type pass_type,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) = 0;

	virtual void set_color(const st_vec3f& color) {}

	bool supports_pass(e_st_render_pass_type type) const
	{
		return bool(static_cast<e_st_render_pass_type_flags>(_supported_passes) & type);
	}

protected:
	uint64_t _supported_passes = 0;
};

struct st_view_cb
{
	st_mat4f _mvp;
};
