#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/st_texture.h>

#include <math/st_mat4f.h>
#include <math/st_vec3f.h>

#include <memory>
#include <string>

#include <wrl.h>

// For the record, I'm not fond of this direction.
enum st_material_type
{
	st_material_type_unlit_texture,
	st_material_type_constant_color,
	st_material_type_phong,
	st_material_type_font,
	st_material_type_gbuffer,
	st_material_type_fullscreen,
	st_material_type_deferred_light,
	st_material_type_tonemap,
	st_material_type_bloom,
	st_material_type_gaussian_blur_vertical,
	st_material_type_gaussian_blur_horizontal,
	st_material_type_parallax_occlusion,
};

/*
** Base class for all graphical materials.
** Includes the shaders and other state necessary to draw geometry.
*/
class st_material
{
public:
	virtual void bind(
		class st_render_context* context,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) = 0;

	virtual void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) = 0;

	virtual void set_color(const st_vec3f& color) {}
	virtual st_material_type get_material_type() = 0;
};

struct st_view_cb
{
	st_mat4f _mvp;
};
