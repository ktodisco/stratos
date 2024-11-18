#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_material.h>

class st_bloom_threshold_material : public st_material
{
public:
	st_bloom_threshold_material(
		class st_render_texture* source,
		class st_render_texture* target,
		struct st_vertex_format* vertex_format,
		struct st_render_pass* pass,
		float cutoff);
	~st_bloom_threshold_material();

	void bind(
		class st_graphics_context* context,
		enum e_st_render_pass_type pass_type,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

private:
	float _cutoff = 0.0f;
	class st_render_texture* _texture;
	std::unique_ptr<struct st_buffer> _cb = nullptr;
	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;

	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;
};

class st_bloom_downsample_material : public st_material
{
public:
	st_bloom_downsample_material(
		class st_render_texture* source,
		class st_render_texture* target,
		struct st_vertex_format* vertex_format,
		struct st_render_pass* pass);
	~st_bloom_downsample_material();

	void bind(
		class st_graphics_context* context,
		enum e_st_render_pass_type pass_type,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

private:
	class st_render_texture* _texture;
	st_vec2f _target_size = st_vec2f::zero_vector();
	std::unique_ptr<struct st_buffer> _cb = nullptr;
	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;

	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;
};

class st_bloom_upsample_material : public st_material
{
public:
	st_bloom_upsample_material(
		class st_render_texture* blur,
		class st_render_texture* step,
		class st_render_texture* target,
		struct st_vertex_format* vertex_format,
		struct st_render_pass* pass);
	~st_bloom_upsample_material();

	void bind(
		class st_graphics_context* context,
		enum e_st_render_pass_type pass_type,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

private:
	class st_render_texture* _blur;
	class st_render_texture* _step;
	std::unique_ptr<struct st_buffer> _cb = nullptr;
	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;

	std::unique_ptr<struct st_pipeline> _pipeline = nullptr;
};
