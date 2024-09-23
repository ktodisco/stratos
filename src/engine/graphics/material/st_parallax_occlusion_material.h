#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_material.h>

struct st_parallax_occlusion_cb
{
	st_mat4f _model;
	st_mat4f _mvp;
	st_vec4f _eye;
};

/*
** Material for objects drawn with the parallax occlusion mapping effect.
*/
class st_parallax_occlusion_material : public st_material
{
public:
	st_parallax_occlusion_material(
		const char* albedo_texture,
		const char* normal_texture);
	~st_parallax_occlusion_material();

	virtual void bind(
		class st_graphics_context* context,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_parallax_occlusion; }

private:
	std::unique_ptr<struct st_buffer> _parallax_occlusion_buffer = nullptr;
	std::unique_ptr<struct st_texture> _albedo_texture;
	std::unique_ptr<struct st_texture> _normal_texture;

	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;
};
