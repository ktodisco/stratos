#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_material.h>

/*
** A material supporting the phong shading model.
*/
class st_phong_material final : public st_material
{
public:
	st_phong_material();
	~st_phong_material();

	void bind(
		class st_render_context* context,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_phong; }

private:
	std::unique_ptr<class st_constant_buffer> _phong_buffer = nullptr;

	std::unique_ptr<class st_resource_table> _resource_table = nullptr;
};
