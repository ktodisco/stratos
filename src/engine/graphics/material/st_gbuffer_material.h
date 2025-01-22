#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_material.h>

#include <graphics/st_pipeline_state_desc.h>

struct st_shadow_cb
{
	st_mat4f _mvp;
};

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
		enum e_st_render_pass_type pass_type,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void set_emissive(float e) { _emissive = e; }

private:
	std::unique_ptr<struct st_buffer> _gbuffer_buffer = nullptr;
	std::unique_ptr<struct st_buffer_view> _gbv = nullptr;
	std::unique_ptr<struct st_buffer> _shadow_buffer = nullptr;
	std::unique_ptr<struct st_buffer_view> _sbv = nullptr;

	std::unique_ptr<struct st_texture> _albedo_texture;
	std::unique_ptr<struct st_texture_view> _albedo_view;
	std::unique_ptr<struct st_texture> _mre_texture;
	std::unique_ptr<struct st_texture_view> _mre_view;

	std::unique_ptr<struct st_vertex_format> _vertex_format = nullptr;
	std::unique_ptr<struct st_pipeline> _gbuffer_pipeline = nullptr;
	std::unique_ptr<struct st_pipeline> _shadow_pipeline = nullptr;

	std::unique_ptr<struct st_resource_table> _gbuffer_resources = nullptr;
	std::unique_ptr<struct st_resource_table> _shadow_resources = nullptr;

	float _emissive = 0;
};
