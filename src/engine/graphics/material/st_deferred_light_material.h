#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_material.h>

class st_deferred_light_material : public st_material
{
public:
	st_deferred_light_material(
		class st_render_texture* albedo_texture,
		class st_render_texture* normal_texture,
		class st_render_texture* third_texture,
		class st_render_texture* depth_texture,
		struct st_buffer* constants,
		struct st_buffer* light_buffer);
	~st_deferred_light_material();

	virtual void bind(
		class st_graphics_context* context,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_deferred_light; }

private:
	class st_render_texture* _albedo;
	class st_render_texture* _normal;
	class st_render_texture* _third;
	class st_render_texture* _depth;

	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;
};
