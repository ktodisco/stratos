/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/dx12/st_dx12_pipeline_state.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/dx12/st_dx12_render_context.h>
#include <graphics/dx12/st_dx12_shader.h>
#include <graphics/dx12/st_dx12_vertex_format.h>

#include <graphics/st_pipeline_state.h>
#include <graphics/st_shader.h>
#include <graphics/st_vertex_format.h>

#include <graphics/dx12/d3dx12.h>

st_dx12_pipeline_state::st_dx12_pipeline_state(const st_pipeline_state_desc& desc)
{
	// Construct the DX12 pipeline state description using our internal representation.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_desc = {};

	// Set the root signature from the graphics context.
	pipeline_desc.pRootSignature = st_dx12_render_context::get()->get_root_signature();

	// Get the input layout from the vertex format.
	pipeline_desc.InputLayout = desc._vertex_format->get_layout();

	// Get the shader bytecodes.
	// Ordinarily I'd use ternary operators here, but that generated code which would call
	// the shader bytecode helper constructor with a null blob.
	if (desc._shader->has_vertex()) { pipeline_desc.VS = CD3DX12_SHADER_BYTECODE(desc._shader->get_vertex_blob()); }
	if (desc._shader->has_pixel()) { pipeline_desc.PS = CD3DX12_SHADER_BYTECODE(desc._shader->get_pixel_blob()); }
	if (desc._shader->has_domain()) { pipeline_desc.DS = CD3DX12_SHADER_BYTECODE(desc._shader->get_domain_blob()); }
	if (desc._shader->has_hull()) { pipeline_desc.HS = CD3DX12_SHADER_BYTECODE(desc._shader->get_hull_blob()); }
	if (desc._shader->has_geometry()) { pipeline_desc.GS = CD3DX12_SHADER_BYTECODE(desc._shader->get_geometry_blob()); }

	pipeline_desc.SampleMask = desc._sample_mask;

	// Rasterizer state.
	D3D12_RASTERIZER_DESC raster_desc = {};
	raster_desc.CullMode = (D3D12_CULL_MODE)desc._rasterizer_desc._cull_mode;
	raster_desc.FillMode = (D3D12_FILL_MODE)desc._rasterizer_desc._fill_mode;
	raster_desc.FrontCounterClockwise = !desc._rasterizer_desc._winding_order_clockwise;

	pipeline_desc.RasterizerState = raster_desc;

	// Blend state.
	D3D12_BLEND_DESC blend_desc = {};
	blend_desc.AlphaToCoverageEnable = desc._blend_desc._alpha_to_coverage;
	blend_desc.IndependentBlendEnable = desc._blend_desc._independent_blend;
	for (uint32_t target_itr = 0; target_itr < desc._render_target_count; ++target_itr)
	{
		D3D12_RENDER_TARGET_BLEND_DESC target_blend = {};
		target_blend.BlendEnable = desc._blend_desc._target_blend[target_itr]._blend;
		target_blend.BlendOp = (D3D12_BLEND_OP)desc._blend_desc._target_blend[target_itr]._blend_op;
		target_blend.BlendOpAlpha = (D3D12_BLEND_OP)desc._blend_desc._target_blend[target_itr]._blend_op_alpha;
		target_blend.DestBlend = (D3D12_BLEND)desc._blend_desc._target_blend[target_itr]._dst_blend;
		target_blend.DestBlendAlpha = (D3D12_BLEND)desc._blend_desc._target_blend[target_itr]._dst_blend_alpha;
		target_blend.LogicOp = (D3D12_LOGIC_OP)desc._blend_desc._target_blend[target_itr]._logic_op;
		target_blend.LogicOpEnable = desc._blend_desc._target_blend[target_itr]._logic;
		target_blend.RenderTargetWriteMask = desc._blend_desc._target_blend[target_itr]._write_mask;
		target_blend.SrcBlend = (D3D12_BLEND)desc._blend_desc._target_blend[target_itr]._src_blend;
		target_blend.SrcBlendAlpha = (D3D12_BLEND)desc._blend_desc._target_blend[target_itr]._src_blend_alpha;

		blend_desc.RenderTarget[target_itr] = target_blend;
	}

	pipeline_desc.BlendState = blend_desc;

	// Depth/stencil state.
	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = desc._depth_stencil_desc._depth_enable;
	depth_stencil_desc.DepthWriteMask = (D3D12_DEPTH_WRITE_MASK)desc._depth_stencil_desc._depth_mask;
	depth_stencil_desc.DepthFunc = (D3D12_COMPARISON_FUNC)desc._depth_stencil_desc._depth_compare;
	depth_stencil_desc.StencilEnable = desc._depth_stencil_desc._stencil_enable;
	depth_stencil_desc.StencilReadMask = desc._depth_stencil_desc._stencil_read_mask;
	depth_stencil_desc.StencilWriteMask = desc._depth_stencil_desc._stencil_write_mask;

	D3D12_DEPTH_STENCILOP_DESC front_desc = {};
	front_desc.StencilFailOp = (D3D12_STENCIL_OP)desc._depth_stencil_desc._front_stencil._stencil_fail_op;
	front_desc.StencilDepthFailOp = (D3D12_STENCIL_OP)desc._depth_stencil_desc._front_stencil._depth_fail_op;
	front_desc.StencilPassOp = (D3D12_STENCIL_OP)desc._depth_stencil_desc._front_stencil._stencil_pass_op;
	front_desc.StencilFunc = (D3D12_COMPARISON_FUNC)desc._depth_stencil_desc._front_stencil._stencil_func;
	depth_stencil_desc.FrontFace = front_desc;

	D3D12_DEPTH_STENCILOP_DESC back_desc = {};
	back_desc.StencilFailOp = (D3D12_STENCIL_OP)desc._depth_stencil_desc._back_stencil._stencil_fail_op;
	back_desc.StencilDepthFailOp = (D3D12_STENCIL_OP)desc._depth_stencil_desc._back_stencil._depth_fail_op;
	back_desc.StencilPassOp = (D3D12_STENCIL_OP)desc._depth_stencil_desc._back_stencil._stencil_pass_op;
	back_desc.StencilFunc = (D3D12_COMPARISON_FUNC)desc._depth_stencil_desc._back_stencil._stencil_func;
	depth_stencil_desc.BackFace = back_desc;

	pipeline_desc.DepthStencilState = depth_stencil_desc;

	// Primitive topology.
	pipeline_desc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE)desc._primitive_topology_type;

	// Render targets.
	pipeline_desc.NumRenderTargets = desc._render_target_count;
	for (uint32_t target_itr = 0; target_itr < desc._render_target_count; ++target_itr)
	{
		pipeline_desc.RTVFormats[target_itr] = (DXGI_FORMAT)desc._render_target_formats[target_itr];
	}

	// Depth/stencil format.
	pipeline_desc.DSVFormat = (DXGI_FORMAT)desc._depth_stencil_format;

	// Multisampling.
	pipeline_desc.SampleDesc.Count = desc._sample_desc._count;
	pipeline_desc.SampleDesc.Quality = desc._sample_desc._quality;

	// Create the pipeline state object.
	st_dx12_render_context::get()->create_graphics_pipeline_state(pipeline_desc, _pipeline_state.GetAddressOf());
}

st_dx12_pipeline_state::~st_dx12_pipeline_state()
{
}

#endif
