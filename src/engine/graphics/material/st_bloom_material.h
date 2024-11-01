#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_material.h>

class st_bloom_material : public st_material
{
public:
	st_bloom_material(
		class st_render_texture* source,
		class st_render_texture* target,
		struct st_vertex_format* vertex_format,
		struct st_render_pass* pass);
	~st_bloom_material();

	void bind(
		class st_graphics_context* context,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

private:
	struct st_texture* _texture;
	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;

	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;
};
