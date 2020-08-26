/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_framebuffer.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>
#include <graphics/platform/vulkan/st_vk_render_pass.h>

#include <graphics/st_render_context.h>
#include <graphics/st_render_texture.h>

#include <vector>

st_vk_framebuffer::st_vk_framebuffer(
	const vk::RenderPass& pass,
	uint32_t count,
	st_render_texture** targets,
	st_render_texture* depth_stencil)
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	uint32_t attachmentCount = count + (depth_stencil ? 1 : 0);

	_width = targets[0]->get_width();
	_height = targets[0]->get_height();

	std::vector<vk::ImageView> views;
	views.reserve(count + 1);

	for (int i = 0; i < count; ++i)
	{
		views.push_back(targets[i]->get_resource_view());
		_targets.push_back(targets[i]);
	}

	if (depth_stencil)
	{
		views.push_back(depth_stencil->get_resource_view());
		_depth_stencil = depth_stencil;
	}

	vk::FramebufferCreateInfo create_info = vk::FramebufferCreateInfo()
		.setAttachmentCount(attachmentCount)
		.setPAttachments(views.data())
		.setRenderPass(pass)
		.setWidth(_width)
		.setHeight(_height)
		.setLayers(1);

	device->createFramebuffer(&create_info, nullptr, &_framebuffer);
}

st_vk_framebuffer::~st_vk_framebuffer()
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	device->destroyFramebuffer(_framebuffer, nullptr);
}

void st_vk_framebuffer::transition(st_render_context* context)
{
	for (uint32_t i = 0; i < _targets.size(); ++i)
	{
		_targets[i]->transition(context, st_texture_state_render_target);
	}

	if (_depth_stencil)
	{
		_depth_stencil->transition(context, st_texture_state_depth_target);
	}
}

#endif
