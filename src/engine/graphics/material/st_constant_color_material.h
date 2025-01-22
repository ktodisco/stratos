#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_material.h>

/*
** Simple untextured material with a constant color.
*/
class st_constant_color_material : public st_material
{
public:
	st_constant_color_material();
	~st_constant_color_material();

	//virtual bool init() override;

	virtual void bind(
		class st_graphics_context* context,
		enum e_st_render_pass_type pass_type,
		const struct st_frame_params* params,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void set_color(const st_vec3f& color) override { _color = color; }

	struct st_constant_color_cb
	{
		st_mat4f _mvp;
		st_vec3f _color;
	};

private:
	std::unique_ptr<struct st_buffer> _color_buffer = nullptr;
	std::unique_ptr<struct st_buffer_view> _cbv = nullptr;
	st_vec3f _color = { 0, 0, 0 };

	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;
};
