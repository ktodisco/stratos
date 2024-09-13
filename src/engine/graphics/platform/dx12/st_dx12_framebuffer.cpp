/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_framebuffer.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/st_render_context.h>
#include <graphics/st_render_texture.h>

st_dx12_framebuffer::st_dx12_framebuffer(
	uint32_t count,
	st_render_texture** targets,
	st_render_texture* depth_stencil) :
	_target_count(count)
{
	for (uint32_t target_itr = 0; target_itr < _target_count; target_itr++)
	{
		_targets.push_back(targets[target_itr]);
	}

	_depth_stencil = depth_stencil;
}

st_dx12_framebuffer::~st_dx12_framebuffer()
{
	_targets.clear();
}

void st_dx12_framebuffer::bind(st_render_context* context)
{
	std::vector<const st_texture_view*> views;
	for (auto& t : _targets)
	{
		context->transition(t->get_texture(), st_texture_state_render_target);
		views.push_back(t->get_view());
	}

	const st_texture_view* ds_view = nullptr;
	if (_depth_stencil)
	{
		context->transition(_depth_stencil->get_texture(), st_texture_state_depth_target);
		ds_view = _depth_stencil->get_view();
	}

	// Bind them.
	context->set_render_targets(_target_count, views.data(), ds_view);
}

void st_dx12_framebuffer::unbind(st_render_context* context)
{
	for (auto& t : _targets)
	{
		context->transition(t->get_texture(), st_texture_state_pixel_shader_read);
	}

	if (_depth_stencil)
	{
		context->transition(_depth_stencil->get_texture(), st_texture_state_pixel_shader_read);
	}

	context->set_render_targets(0, nullptr, nullptr);
}

#endif
