/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_render_pass.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_framebuffer.h>
#include <graphics/platform/vulkan/st_vk_render_texture.h>

#include <graphics/st_render_context.h>
#include <graphics/st_render_texture.h>

#include <vector>

st_vk_render_pass::st_vk_render_pass(
	uint32_t count,
	const st_render_texture** targets,
	const st_render_texture* depth_stencil)
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	// Naively, create the viewport from the first target.
	if (count > 0)
	{
		_viewport = vk::Viewport()
			.setX(0)
			.setY(0)
			.setWidth(targets[0]->get_width())
			.setHeight(targets[0]->get_height());
	}

	std::vector<vk::AttachmentDescription> attachment_descs;
	std::vector<vk::AttachmentReference> attachment_refs;
	for (int i = 0; i < count; ++i)
	{
		vk::AttachmentDescription desc = vk::AttachmentDescription()
			.setFormat(vk::Format(targets[i]->get_format()))
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eLoad)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
		attachment_descs.push_back(desc);

		vk::AttachmentReference ref = vk::AttachmentReference()
			.setAttachment(i)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
		attachment_refs.push_back(ref);
	}

	vk::SubpassDescription subpass_desc = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setInputAttachmentCount(0)
		.setColorAttachmentCount(count)
		.setPColorAttachments(attachment_refs.data());

	if (depth_stencil)
	{
		vk::AttachmentDescription ds_desc = vk::AttachmentDescription()
			.setFormat(vk::Format(depth_stencil->get_format()))
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eLoad)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
		attachment_descs.push_back(ds_desc);

		vk::AttachmentReference ds_ref = vk::AttachmentReference()
			.setAttachment(attachment_descs.size() - 1)
			.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		subpass_desc.setPDepthStencilAttachment(&ds_ref);
	}

	vk::RenderPassCreateInfo pass_info = vk::RenderPassCreateInfo()
		.setAttachmentCount(attachment_descs.size())
		.setPAttachments(attachment_descs.data())
		.setSubpassCount(1)
		.setPSubpasses(&subpass_desc)
		.setDependencyCount(0);

	VK_VALIDATE(device->createRenderPass(&pass_info, nullptr, &_render_pass));

	_framebuffer = std::make_unique<st_vk_framebuffer>(
		_render_pass,
		count,
		targets,
		depth_stencil);
}

st_vk_render_pass::~st_vk_render_pass()
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	device->destroyRenderPass(_render_pass, nullptr);
	_framebuffer = nullptr;
}

void st_vk_render_pass::begin(st_render_context* context)
{
	vk::CommandBuffer* command_buffer = context->get_command_buffer();

	vk::RenderPassBeginInfo begin_info = vk::RenderPassBeginInfo()
		.setClearValueCount(0)
		.setFramebuffer(_framebuffer->get())
		.setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(_framebuffer->get_width(), _framebuffer->get_height())))
		.setRenderPass(_render_pass);

	command_buffer->beginRenderPass(&begin_info, vk::SubpassContents::eInline);
}

void st_vk_render_pass::end(st_render_context* context)
{
	vk::CommandBuffer* command_buffer = context->get_command_buffer();

	command_buffer->endRenderPass();
}

#endif
