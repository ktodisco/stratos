/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_framebuffer.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>

#include <graphics/st_render_texture.h>

#include <vector>

st_vk_framebuffer::st_vk_framebuffer(
	vk::Device* device,
	const vk::RenderPass& pass,
	uint32_t count,
	st_render_texture** targets,
	st_render_texture* depth_stencil)
{
	_device = device;

	uint32_t attachmentCount = count + (depth_stencil ? 1 : 0);

	_width = targets[0]->get_width();
	_height = targets[0]->get_height();

	std::vector<vk::ImageView> views;
	views.reserve(count + 1);

	for (int i = 0; i < count; ++i)
	{
		st_vk_texture* target = static_cast<st_vk_texture*>(targets[i]->get_texture());

		views.push_back(target->_view);
		_targets.push_back(targets[i]);
	}

	if (depth_stencil)
	{
		st_vk_texture* ds = static_cast<st_vk_texture*>(depth_stencil->get_texture());

		views.push_back(ds->_view);
		_depth_stencil = depth_stencil;
	}

	vk::FramebufferCreateInfo create_info = vk::FramebufferCreateInfo()
		.setAttachmentCount(attachmentCount)
		.setPAttachments(views.data())
		.setRenderPass(pass)
		.setWidth(_width)
		.setHeight(_height)
		.setLayers(1);

	VK_VALIDATE(_device->createFramebuffer(&create_info, nullptr, &_framebuffer));
}

st_vk_framebuffer::~st_vk_framebuffer()
{
	_device->destroyFramebuffer(_framebuffer, nullptr);
}

void st_vk_framebuffer::transition(st_render_context* context)
{
	for (uint32_t i = 0; i < _targets.size(); ++i)
	{
		context->transition(_targets[i]->get_texture(), st_texture_state_render_target);
	}

	if (_depth_stencil)
	{
		context->transition(_depth_stencil->get_texture(), st_texture_state_depth_stencil_target);
	}
}

#endif
