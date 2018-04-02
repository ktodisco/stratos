/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/dx12/st_dx12_framebuffer.h>

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
	// Transition the render textures to render targets.
	std::vector<D3D12_RESOURCE_BARRIER> barriers;

	for (auto& t : _targets)
	{
		barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
			t->get_resource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET));
	}

	if (_depth_stencil)
	{
		barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
			_depth_stencil->get_resource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_DEPTH_WRITE));
	}

	context->transition_targets(barriers.size(), &barriers[0]);

	// Bind them.
	context->set_render_targets(_target_count, &_targets[0], _depth_stencil);
}

void st_dx12_framebuffer::unbind(st_render_context* context)
{
	// Transition the render textures to shader resources.
	std::vector<D3D12_RESOURCE_BARRIER> barriers;

	for (auto& t : _targets)
	{
		barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
			t->get_resource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}

	if (_depth_stencil)
	{
		barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
			_depth_stencil->get_resource(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}

	context->transition_targets(barriers.size(), &barriers[0]);

	context->set_render_targets(0, nullptr, nullptr);
}

#endif
