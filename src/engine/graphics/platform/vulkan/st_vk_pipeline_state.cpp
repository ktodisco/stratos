/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_pipeline_state.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_pass.h>
#include <graphics/st_shader.h>
#include <graphics/geometry/st_vertex_format.h>

#include <vector>

st_vk_pipeline_state::st_vk_pipeline_state(
	const st_pipeline_state_desc& desc,
	const st_render_pass* render_pass)
{
	std::vector<vk::PipelineShaderStageCreateInfo> stages;

	if (desc._shader->has_vertex())
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(desc._shader->get_vertex_module());
	}
	if (desc._shader->has_pixel())
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(desc._shader->get_pixel_module());
	}
	if (desc._shader->has_domain())
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eTessellationEvaluation)
			.setModule(desc._shader->get_domain_module());
	}
	if (desc._shader->has_hull())
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eTessellationControl)
			.setModule(desc._shader->get_hull_module());
	}
	if (desc._shader->has_geometry())
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eGeometry)
			.setModule(desc._shader->get_geometry_module());
	}

	vk::PipelineInputAssemblyStateCreateInfo input_assembly = vk::PipelineInputAssemblyStateCreateInfo()
		.setTopology(vk::PrimitiveTopology(desc._primitive_topology_type));

	vk::Rect2D scissor = vk::Rect2D()
		.setOffset(vk::Offset2D())
		.setExtent(vk::Extent2D(render_pass->get_viewport().x, render_pass->get_viewport().y));

	vk::PipelineViewportStateCreateInfo viewport = vk::PipelineViewportStateCreateInfo()
		.setViewportCount(1)
		.setPViewports(&render_pass->get_viewport())
		.setScissorCount(1)
		.setPScissors(&scissor);

	vk::PipelineRasterizationStateCreateInfo raster = vk::PipelineRasterizationStateCreateInfo()
		.setCullMode(vk::CullModeFlags(desc._rasterizer_desc._cull_mode))
		.setPolygonMode(vk::PolygonMode(desc._rasterizer_desc._fill_mode))
		.setFrontFace(vk::FrontFace(desc._rasterizer_desc._winding_order_clockwise))
		.setDepthClampEnable(false)
		.setRasterizerDiscardEnable(false)
		.setDepthBiasEnable(false);

	vk::PipelineMultisampleStateCreateInfo multisample = vk::PipelineMultisampleStateCreateInfo()
		.setAlphaToCoverageEnable(desc._blend_desc._alpha_to_coverage)
		.setAlphaToOneEnable(false)
		.setRasterizationSamples(vk::SampleCountFlagBits::e1)
		.setSampleShadingEnable(false);

	vk::StencilOpState stencil_front = vk::StencilOpState()
		.setFailOp(vk::StencilOp(desc._depth_stencil_desc._front_stencil._stencil_fail_op))
		.setDepthFailOp(vk::StencilOp(desc._depth_stencil_desc._front_stencil._depth_fail_op))
		.setPassOp(vk::StencilOp(desc._depth_stencil_desc._front_stencil._stencil_pass_op))
		.setCompareOp(vk::CompareOp(desc._depth_stencil_desc._front_stencil._stencil_func));

	vk::StencilOpState stencil_back = vk::StencilOpState()
		.setFailOp(vk::StencilOp(desc._depth_stencil_desc._back_stencil._stencil_fail_op))
		.setDepthFailOp(vk::StencilOp(desc._depth_stencil_desc._back_stencil._depth_fail_op))
		.setPassOp(vk::StencilOp(desc._depth_stencil_desc._back_stencil._stencil_pass_op))
		.setCompareOp(vk::CompareOp(desc._depth_stencil_desc._back_stencil._stencil_func));

	vk::PipelineDepthStencilStateCreateInfo depth_stencil = vk::PipelineDepthStencilStateCreateInfo()
		.setDepthTestEnable(desc._depth_stencil_desc._depth_enable)
		.setDepthWriteEnable(desc._depth_stencil_desc._depth_mask)
		.setDepthCompareOp(vk::CompareOp(desc._depth_stencil_desc._depth_compare))
		.setDepthBoundsTestEnable(false)
		.setStencilTestEnable(desc._depth_stencil_desc._stencil_enable)
		.setFront(stencil_front)
		.setBack(stencil_back);

	std::vector<vk::PipelineColorBlendAttachmentState> attachment_states;
	for (int i = 0; i < desc._render_target_count; ++i)
	{
		vk::PipelineColorBlendAttachmentState state = vk::PipelineColorBlendAttachmentState()
			.setBlendEnable(desc._blend_desc._target_blend[i]._blend)
			.setColorBlendOp(vk::BlendOp(desc._blend_desc._target_blend[i]._blend_op))
			.setSrcColorBlendFactor(vk::BlendFactor(desc._blend_desc._target_blend[i]._src_blend))
			.setDstColorBlendFactor(vk::BlendFactor(desc._blend_desc._target_blend[i]._dst_blend))
			.setAlphaBlendOp(vk::BlendOp(desc._blend_desc._target_blend[i]._blend_op_alpha))
			.setSrcAlphaBlendFactor(vk::BlendFactor(desc._blend_desc._target_blend[i]._src_blend_alpha))
			.setDstAlphaBlendFactor(vk::BlendFactor(desc._blend_desc._target_blend[i]._dst_blend_alpha))
			.setColorWriteMask(vk::ColorComponentFlags(desc._blend_desc._target_blend[i]._write_mask));

		attachment_states.push_back(state);
	}

	vk::PipelineColorBlendStateCreateInfo color_blend = vk::PipelineColorBlendStateCreateInfo()
		.setAttachmentCount(desc._render_target_count)
		.setPAttachments(attachment_states.data())
		.setLogicOpEnable(false);

	// TODO: Set the descriptor set layout.
	vk::GraphicsPipelineCreateInfo create_info = vk::GraphicsPipelineCreateInfo()
		.setStageCount(stages.size())
		.setPStages(stages.data())
		.setPVertexInputState(&desc._vertex_format->get_layout())
		.setPInputAssemblyState(&input_assembly)
		.setPViewportState(&viewport)
		.setRenderPass(render_pass->get())
		.setPRasterizationState(&raster)
		.setPMultisampleState(&multisample)
		.setPDepthStencilState(&depth_stencil)
		.setPColorBlendState(&color_blend)
		.setSubpass(0);

	vk::Device* device = st_vk_render_context::get()->get_device();

	VK_VALIDATE(device->createGraphicsPipelines(vk::PipelineCache(nullptr), 1, &create_info, nullptr, &_pipeline));
}

st_vk_pipeline_state::~st_vk_pipeline_state()
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	device->destroyPipeline(_pipeline);
}

#endif