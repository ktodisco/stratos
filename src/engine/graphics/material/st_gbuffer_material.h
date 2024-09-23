#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_material.h>

#include <graphics/st_pipeline_state_desc.h>

struct st_gbuffer_cb
{
	st_mat4f _model;
	st_mat4f _mvp;
	float _emissive;
};

/*
** Material for objects drawn to the gbuffer.
*/
class st_gbuffer_material : public st_material
{
public:
	st_gbuffer_material(
		const char* albedo_texture,
		const char* mre_texture);
	~st_gbuffer_material();

	virtual void bind(
		class st_graphics_context* context,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_gbuffer; }

	void set_emissive(float e) { _emissive = e; }

private:
	std::unique_ptr<struct st_buffer> _gbuffer_buffer = nullptr;
	std::unique_ptr<struct st_texture> _albedo_texture;
	std::unique_ptr<struct st_texture> _mre_texture;

	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;

	float _emissive = 0;
};
