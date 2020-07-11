/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_framebuffer.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>
#include <graphics/platform/vulkan/st_vk_render_pass.h>

#include <graphics/st_render_texture.h>

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

	vk::FramebufferCreateInfo create_info = vk::FramebufferCreateInfo()
		.setAttachmentCount(attachmentCount)
		//.setPAttachments(nullptr)
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

#endif
