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
	st_gaussian_blur_vertical_material(
		class st_render_texture* texture,
		class st_render_texture* target,
		struct st_vertex_format* vertex_format,
		struct st_render_pass* pass);
	~st_gaussian_blur_vertical_material();

	virtual void bind(
		class st_graphics_context* context,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

private:
	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;

	class st_render_texture* _texture;
	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;
};

class st_gaussian_blur_horizontal_material : public st_material
{
public:
	st_gaussian_blur_horizontal_material(
		class st_render_texture* texture,
		class st_render_texture* target,
		struct st_vertex_format* vertex_format,
		struct st_render_pass* pass);
	~st_gaussian_blur_horizontal_material();

	virtual void bind(
		class st_graphics_context* context,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

private:
	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;

	class st_render_texture* _texture;
	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;
};
