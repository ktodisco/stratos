#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_material.h>

/*
** Material for objects drawn to the gbuffer.
*/
class st_gbuffer_material : public st_material
{
public:
	st_gbuffer_material(const char* texture_file);
	~st_gbuffer_material();

	virtual void bind(
		class st_render_context* context,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_gbuffer; }

private:
	// TODO: This view buffer should live at a larger scope.
	std::unique_ptr<class st_constant_buffer> _view_buffer = nullptr;
	std::string _texture_file;
	std::unique_ptr<st_texture> _texture;
};