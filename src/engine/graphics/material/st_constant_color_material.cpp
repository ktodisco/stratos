/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_constant_color_material.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_shader_manager.h>

st_constant_color_material::st_constant_color_material() :
	st_material(e_st_render_pass_type::ui)
{
	st_graphics_context* context = st_graphics_context::get();

	st_buffer_desc desc;
	desc._count = 1;
	desc._element_size = sizeof(st_constant_color_cb);
	desc._usage = e_st_buffer_usage::uniform;
	_color_buffer = context->create_buffer(desc);
	context->add_constant(_color_buffer.get(), "type_cb0", st_shader_constant_type_block);

	_resource_table = context->create_resource_table();
	st_buffer* cbs[] = { _color_buffer.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
}

st_constant_color_material::~st_constant_color_material()
{
}

void st_constant_color_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;

	st_constant_color_cb cb_data{};
	cb_data._mvp = mvp;
	cb_data._color = _color;
	context->update_buffer(_color_buffer.get(), &cb_data, 0, 1);

	context->bind_resources(_resource_table.get());
}
