#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_material.h>

/*
** Material for a passthrough fullscreen pass.
*/
class st_fullscreen_material : public st_material
{
public:
	st_fullscreen_material(
		class st_render_texture* texture,
		class st_render_texture* target,
		struct st_vertex_format* vertex_format,
		struct st_render_pass* pass);
	~st_fullscreen_material();

	virtual void bind(
		class st_graphics_context* context,
		enum e_st_render_pass_type pass_type,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

private:
	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;

	class st_render_texture* _texture;
	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;
};
