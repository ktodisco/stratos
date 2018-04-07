/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_deferred_light_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/st_constant_buffer.h>
#include <graphics/st_deferred_light_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_framebuffer.h>
#include <graphics/st_geometry.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_point_light.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_vertex_format.h>

st_deferred_light_render_pass::st_deferred_light_render_pass(
	st_render_texture* albedo_buffer,
	st_render_texture* normal_buffer,
	st_render_texture* depth_buffer,
	st_render_texture* output_buffer,
	st_render_texture* output_depth)
{
	_vertex_format = std::make_unique<st_vertex_format>();
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	_vertex_format->finalize();

	const float verts[] =
	{
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
	};

	const uint16_t indices[] =
	{
		2, 1, 0,
		3, 2, 0,
	};

	_fullscreen_quad = std::make_unique<st_geometry>(
		_vertex_format.get(),
		(void*)verts,
		static_cast<uint32_t>(sizeof(float) * 3),
		12,
		(uint16_t*)indices,
		6);

	_light_buffer = std::make_unique<st_constant_buffer>(sizeof(st_point_light_cb));
	// TODO: These should be validated against the buffer's provided size.
	_light_buffer->add_constant("u_inverse_vp", st_shader_constant_type_mat4);
	_light_buffer->add_constant("u_eye", st_shader_constant_type_vec4);
	_light_buffer->add_constant("u_light_position", st_shader_constant_type_vec4);
	_light_buffer->add_constant("u_light_color", st_shader_constant_type_vec4);
	_light_buffer->add_constant("u_light_power", st_shader_constant_type_float);

	_default_material = std::make_unique<st_deferred_light_material>(
		albedo_buffer,
		normal_buffer,
		depth_buffer,
		_light_buffer.get());

	st_pipeline_state_desc deferred_light_state_desc;
	_default_material->get_pipeline_state(&deferred_light_state_desc);

	deferred_light_state_desc._vertex_format = _vertex_format.get();
	deferred_light_state_desc._render_target_count = 1;
	deferred_light_state_desc._render_target_formats[0] = output_buffer->get_format();
	deferred_light_state_desc._depth_stencil_format = output_depth->get_format();

	_pipeline_state = std::make_unique<st_pipeline_state>(deferred_light_state_desc);

	st_render_texture* targets[] = { output_buffer };
	_framebuffer = std::make_unique<st_framebuffer>(
		1,
		targets,
		output_depth);

	_light = std::make_unique<st_point_light>(
		st_vec3f({ 10.0f, 10.0f, 10.0f }),
		st_vec3f({ 1.0f, 1.0f, 0.9f }),
		75.0f);
}

st_deferred_light_render_pass::~st_deferred_light_render_pass()
{
}

void st_deferred_light_render_pass::render(
	st_render_context* context,
	const st_frame_params* params)
{
	st_render_marker marker("st_deferred_light_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_pipeline_state(_pipeline_state.get());

	_framebuffer->bind(context);

	// Update the light information.
	st_mat4f perspective;
	perspective.make_perspective_rh(st_degrees_to_radians(45.0f), (float)params->_width / (float)params->_height, 0.1f, 10000.0f);

	st_point_light_cb light_data;
	light_data._inverse_vp = (params->_view * perspective).inverse();
	light_data._inverse_vp.transpose();
	light_data._eye = st_vec4f(params->_eye, 0.0f);
	light_data._position = st_vec4f(_light->get_position(), 0.0f);
	light_data._color = st_vec4f(_light->get_color(), 0.0f);
	light_data._power = _light->get_power();

	_light_buffer->update(context, &light_data);

	_default_material->bind(context, identity, identity, identity);

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);

	context->draw(draw_call);

	_framebuffer->unbind(context);
}
