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
		class st_render_texture* directional_shadow_map,
		class st_render_texture* output_texture,
		const struct st_buffer_view* constants,
		const struct st_buffer_view* light_buffer,
		struct st_vertex_format* vertex_format,
		struct st_render_pass* pass);
	~st_deferred_light_material();

	virtual void bind(
		class st_graphics_context* context,
		enum e_st_render_pass_type pass_type,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

private:
	class st_render_texture* _albedo;
	class st_render_texture* _normal;
	class st_render_texture* _third;
	class st_render_texture* _depth;
	class st_render_texture* _directional_shadow_map;

	std::unique_ptr<struct st_sampler> _shadow_sampler = nullptr;

	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;

	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;
};
