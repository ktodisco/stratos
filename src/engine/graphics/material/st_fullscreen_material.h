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
	st_fullscreen_material(class st_texture* texture);
	~st_fullscreen_material();

	virtual void bind(
		class st_render_context* context,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_fullscreen; }

private:
	class st_texture* _texture;
	std::unique_ptr<class st_resource_table> _resource_table = nullptr;
};
