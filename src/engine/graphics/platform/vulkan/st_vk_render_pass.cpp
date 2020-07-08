/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_render_pass.h>

#include <graphics/platform/vulkan/st_vk_render_context.h>
#include <graphics/platform/vulkan/st_vk_render_texture.h>

#include <graphics/st_render_texture.h>

#include <vector>

st_vk_render_pass::st_vk_render_pass(
	uint32_t count,
	class st_render_texture** targets,
	class st_render_texture* depth_stencil)
{
	vk::Device* device = st_vk_render_context::get()->get_device();

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
	}

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

	vk::SubpassDescription subpass_desc = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setInputAttachmentCount(0)
		.setColorAttachmentCount(count)
		.setPColorAttachments(attachment_refs.data())
		.setPDepthStencilAttachment(&ds_ref);

	vk::RenderPassCreateInfo pass_info = vk::RenderPassCreateInfo()
		.setAttachmentCount(count)
		.setPAttachments(attachment_descs.data())
		.setSubpassCount(1)
		.setPSubpasses(&subpass_desc)
		.setDependencyCount(0);

	device->createRenderPass(&pass_info, nullptr, &_render_pass);

	_framebuffer = std::make_unique<st_framebuffer>(
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

void st_vk_render_pass::begin()
{

}

void st_vk_render_pass::end()
{

}
