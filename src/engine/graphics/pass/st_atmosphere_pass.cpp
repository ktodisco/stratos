/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_atmosphere_pass.h>

#include <entity/st_atmosphere_component.h>

#include <framework/st_frame_params.h>
#include <framework/st_global_resources.h>

#include <graphics/light/st_directional_light.h>

#include <graphics/st_graphics_context.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

#include <functional>

struct st_atmosphere_constants
{
	st_vec4f _radii_dims = st_vec4f::zero_vector();
	st_vec4f _rayleigh_params = st_vec4f::zero_vector();
	st_vec4f _mie_params = st_vec4f::zero_vector();
	st_vec4f _ozone_params = st_vec4f::zero_vector();
	st_vec3f _light_dir = st_vec3f::zero_vector();
	float pad0 = 0.0f;
	st_vec2f _light_angles = st_vec2f::zero_vector();
	float pad1 = 0.0f;
	float pad2 = 0.0f;
	st_mat4f _inverse_p;
	st_mat4f _inverse_v;
};

static void build_constants(st_atmosphere_constants& constants, const st_frame_params* params)
{
	if (!params->_atmosphere)
		return;

	constants._radii_dims = st_vec4f {
		params->_atmosphere->_planet_radius_km * 1000.0f,
		params->_atmosphere->_atmosphere_radius_km * 1000.0f,
		0.0f,
		0.0f
	};
	constants._rayleigh_params = st_vec4f {
		params->_atmosphere->_rayleigh_coefficients.x,
		params->_atmosphere->_rayleigh_coefficients.y,
		params->_atmosphere->_rayleigh_coefficients.z,
		params->_atmosphere->_rayleigh_average * 1000.0f
	};
	constants._mie_params = st_vec4f {
		params->_atmosphere->_mie_coefficients.x,
		params->_atmosphere->_mie_coefficients.y,
		params->_atmosphere->_mie_coefficients.z,
		params->_atmosphere->_mie_average * 1000.0f
	};
	constants._ozone_params = st_vec4f {
		params->_atmosphere->_ozone_coefficients.x,
		params->_atmosphere->_ozone_coefficients.y,
		params->_atmosphere->_ozone_coefficients.z,
		params->_atmosphere->_ozone_average * 1000.0f
	};
	constants._light_dir = params->_sun->_direction;
	constants._light_angles = st_vec2f {
		st_degrees_to_radians(params->_sun_azimuth) / (3.14159f * 2.0f),
		st_degrees_to_radians(params->_sun_angle) / (3.14159f / 2.0f)
	};
	constants._inverse_p = params->_projection.inverse();
	constants._inverse_v = params->_view.inverse();
}

st_atmosphere_transmission_pass::st_atmosphere_transmission_pass(st_render_texture* transmittance)
	: _transmittance(transmittance)
{
	st_graphics_context* context = st_graphics_context::get();

	context->get_desc(_transmittance->get_texture(), &_transmittance_desc);

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_atmosphere_constants);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		_cbv = context->create_buffer_view(desc);
	}

	{
		st_compute_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_atmosphere_transmission);
		_pipeline = context->create_compute_pipeline(desc);
	}

	{
		st_texture_view_desc desc;
		desc._texture = _transmittance->get_texture();
		desc._format = _transmittance->get_format();
		desc._mips = 1;
		desc._usage = e_st_view_usage::unordered_access;
		_uav = context->create_texture_view(desc);
	}

	{
		const st_buffer_view* cbs[] = { _cbv.get() };
		const st_texture_view* uavs[] = { _uav.get() };
		_resources = context->create_resource_table_compute();
		context->set_constant_buffers(_resources.get(), _countof(cbs), cbs);
		context->set_uavs(_resources.get(), _countof(uavs), uavs);
	}
}

st_atmosphere_transmission_pass::~st_atmosphere_transmission_pass()
{
	_cbv = nullptr;
	_cb = nullptr;
	_uav = nullptr;
	_transmittance = nullptr;
	_pipeline = nullptr;
	_resources = nullptr;
}

void st_atmosphere_transmission_pass::compute(st_graphics_context* context, const st_frame_params* params)
{
	// TODO: This does not need to be generated every frame, but to test it a proper
	// hashing utility is needed to hash the atmospheric parameters.

	st_render_marker marker(context, __FUNCTION__);

	st_atmosphere_constants constants;
	build_constants(constants, params);
	constants._radii_dims.z = 1.0f / _transmittance_desc._width;
	constants._radii_dims.w = 1.0f / _transmittance_desc._height;

	context->update_buffer(_cb.get(), &constants, 0, 1);

	context->transition(_transmittance->get_texture(), st_texture_state_unordered_access);

	context->set_compute_pipeline(_pipeline.get());
	context->bind_compute_resources(_resources.get());

	{
		st_dispatch_args args;
		args.group_count_x = (_transmittance_desc._width + 31) / 32;
		args.group_count_y = (_transmittance_desc._height + 31) / 32;
		args.group_count_z = 1;
		context->dispatch(args);
	}
}

st_atmosphere_sky_pass::st_atmosphere_sky_pass(st_render_texture* transmittance, st_render_texture* target)
	: _transmittance(transmittance), _target(target)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_atmosphere_constants);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		_cbv = context->create_buffer_view(desc);
	}

	{
		st_attachment_desc attachments[] =
		{
			{ target->get_format(), e_st_load_op::clear, e_st_store_op::store }
		};
		st_render_pass_desc desc;
		desc._attachments = attachments;
		desc._attachment_count = std::size(attachments);
		desc._viewport = { 0.0f, 0.0f, float(target->get_width()), float(target->get_height()), 0.0f, 1.0f };

		_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc target = { _target->get_texture(), _target->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _pass.get();
		desc._targets = &target;
		desc._target_count = 1;

		_framebuffer = context->create_framebuffer(desc);
	}

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_atmosphere_sky_view);
		desc._vertex_format = _vertex_format.get();
		desc._depth_stencil_desc._depth_enable = false;
		desc._pass = _pass.get();
		desc._render_target_count = 1;
		desc._render_target_formats[0] = target->get_format();
		desc._blend_desc._target_blend[0]._blend = false;
		_pipeline = context->create_graphics_pipeline(desc);
	}

	{
		const st_buffer_view* cbs[] = { _cbv.get() };
		const st_texture_view* srvs[] = { _transmittance->get_resource_view() };
		const st_sampler* samplers[] = { _global_resources->_trilinear_clamp_sampler.get() };
		_resources = context->create_resource_table();
		context->set_constant_buffers(_resources.get(), _countof(cbs), cbs);
		context->set_textures(_resources.get(), _countof(srvs), srvs, samplers);
	}
}

st_atmosphere_sky_pass::~st_atmosphere_sky_pass()
{

}

void st_atmosphere_sky_pass::render(st_graphics_context* context, const st_frame_params* params)
{
	st_render_marker marker(context, __FUNCTION__);

	st_mat4f identity;
	identity.make_identity();

	context->set_scissor(0, 0, params->_width, params->_height);

	st_atmosphere_constants constants;
	build_constants(constants, params);

	context->update_buffer(_cb.get(), &constants, 0, 1);

	context->transition(_transmittance->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_target->get_texture(), st_texture_state_render_target);

	context->set_pipeline(_pipeline.get());
	context->bind_resources(_resources.get());

	st_clear_value clears[] =
	{
		st_vec4f{ 0.0f, 0.0f, 0.0f, 1.0f },
	};

	context->begin_render_pass(_pass.get(), _framebuffer.get(), clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get(), _framebuffer.get());
}

st_atmosphere_render_pass::st_atmosphere_render_pass(
	st_render_texture* target,
	st_render_texture* depth,
	st_render_texture* transmittance,
	st_render_texture* sky_view) :
	_transmittance(transmittance),
	_sky_view(sky_view)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_atmosphere_constants);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		_cbv = context->create_buffer_view(desc);
	}

	{
		st_attachment_desc attachments[] =
		{
			{ target->get_format(), e_st_load_op::load, e_st_store_op::store }
		};
		st_render_pass_desc desc;
		desc._attachments = attachments;
		desc._attachment_count = std::size(attachments);
		desc._depth_attachment = { depth->get_format(), e_st_load_op::load, e_st_store_op::dont_care };
		desc._viewport = { 0.0f, 0.0f, float(target->get_width()), float(target->get_height()), 0.0f, 1.0f };

		_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc t_desc = { target->get_texture(), target->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _pass.get();
		desc._targets = &t_desc;
		desc._target_count = 1;
		desc._depth_target = { depth->get_texture(), depth->get_target_view() };

		_framebuffer = context->create_framebuffer(desc);
	}

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_atmosphere);
		desc._vertex_format = _vertex_format.get();
		desc._depth_stencil_desc._depth_compare = st_compare_func_equal;
		desc._depth_stencil_desc._depth_enable = true;
		desc._depth_stencil_format = depth->get_format();
		desc._pass = _pass.get();
		desc._render_target_count = 1;
		desc._render_target_formats[0] = target->get_format();
		desc._blend_desc._target_blend[0]._blend = true;
		desc._blend_desc._target_blend[0]._blend_op = st_blend_op_add;
		desc._blend_desc._target_blend[0]._src_blend = st_blend_one;
		desc._blend_desc._target_blend[0]._dst_blend = st_blend_zero;
		_pipeline = context->create_graphics_pipeline(desc);
	}

	{
		const st_texture_view* textures[] =
		{
			transmittance->get_resource_view(),
			sky_view->get_resource_view(),
		};
		const st_sampler* samplers[] =
		{
			_global_resources->_trilinear_clamp_sampler.get(),
			_global_resources->_trilinear_clamp_sampler.get()
		};
		const st_buffer_view* cbs[] = { _cbv.get() };
		_resources = context->create_resource_table();
		context->set_constant_buffers(_resources.get(), _countof(cbs), cbs);
		context->set_textures(_resources.get(), _countof(textures), textures, samplers);
	}
}

st_atmosphere_render_pass::~st_atmosphere_render_pass()
{
	_pass = nullptr;
	_resources = nullptr;
	_pipeline = nullptr;
	_cbv = nullptr;
	_cb = nullptr;
}

void st_atmosphere_render_pass::render(class st_graphics_context* context, const st_frame_params* params)
{
	st_render_marker marker(context, __FUNCTION__);

	st_mat4f identity;
	identity.make_identity();

	context->set_scissor(0, 0, params->_width, params->_height);

	st_atmosphere_constants constants;
	build_constants(constants, params);

	context->update_buffer(_cb.get(), &constants, 0, 1);

	context->transition(_sky_view->get_texture(), st_texture_state_pixel_shader_read);

	context->set_pipeline(_pipeline.get());
	context->bind_resources(_resources.get());

	context->begin_render_pass(_pass.get(), _framebuffer.get(), nullptr, 0);

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get(), _framebuffer.get());
}
