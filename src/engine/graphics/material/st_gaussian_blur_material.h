#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_material.h>

class st_gaussian_blur_vertical_material : public st_material
{
public:
	st_gaussian_blur_vertical_material(class st_texture* texture);
	~st_gaussian_blur_vertical_material();

	virtual void bind(
		class st_render_context* context,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_gaussian_blur_vertical; }

private:
	class st_texture* _texture;
	std::unique_ptr<class st_resource_table> _resource_table = nullptr;
};

class st_gaussian_blur_horizontal_material : public st_material
{
public:
	st_gaussian_blur_horizontal_material(class st_texture* texture);
	~st_gaussian_blur_horizontal_material();

	virtual void bind(
		class st_render_context* context,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_gaussian_blur_horizontal; }

private:
	class st_texture* _texture;
	std::unique_ptr<class st_resource_table> _resource_table = nullptr;
};
