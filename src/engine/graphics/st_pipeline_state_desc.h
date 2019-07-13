#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <climits>
#include <cstdint>

struct st_render_target_blend_desc
{
	bool _blend = false;
	bool _logic = false;

	e_st_blend _src_blend = st_blend_zero;
	e_st_blend _dst_blend = st_blend_zero;
	e_st_blend_op _blend_op = st_blend_op_add;

	e_st_blend _src_blend_alpha = st_blend_zero;
	e_st_blend _dst_blend_alpha = st_blend_zero;
	e_st_blend_op _blend_op_alpha = st_blend_op_add;

	uint8_t _write_mask = 0xf;

	e_st_logic_op _logic_op = st_logic_op_noop;
};

struct st_pipeline_blend_desc
{
	bool _alpha_to_coverage = false;
	bool _independent_blend = false;

	st_render_target_blend_desc _target_blend[8] = {};
};

struct st_pipeline_rasterizer_desc
{
	e_st_fill_mode _fill_mode = st_fill_mode_solid;
	e_st_cull_mode _cull_mode = st_cull_mode_back;

	bool _winding_order_clockwise = false;

	// TODO: Rest of D3D12_RASTERIZER_DESC.
};

struct st_pipeline_stencil_op_desc
{
	e_st_stencil_op _stencil_fail_op = st_stencil_op_keep;
	e_st_stencil_op _depth_fail_op = st_stencil_op_keep;
	e_st_stencil_op _stencil_pass_op = st_stencil_op_keep;
	e_st_compare_func _stencil_func = st_compare_func_less;
};

struct st_pipeline_depth_stencil_desc
{
	bool _depth_enable = false;

	e_st_depth_write_mask _depth_mask = st_depth_write_mask_all;
	e_st_compare_func _depth_compare = st_compare_func_less;

	bool _stencil_enable = false;
	uint8_t _stencil_read_mask = 0;
	uint8_t _stencil_write_mask = 0;

	st_pipeline_stencil_op_desc _front_stencil = {};
	st_pipeline_stencil_op_desc _back_stencil = {};
};

struct st_sample_desc
{
	uint32_t _count = 1;
	uint32_t _quality = 0;
};

struct st_pipeline_state_desc
{
	const class st_shader* _shader = {};

	uint32_t _sample_mask = UINT_MAX;

	st_pipeline_blend_desc _blend_desc = {};
	st_pipeline_rasterizer_desc _rasterizer_desc = {};
	st_pipeline_depth_stencil_desc _depth_stencil_desc = {};

	class st_vertex_format* _vertex_format = nullptr;

	e_st_primitive_topology_type _primitive_topology_type = st_primitive_topology_type_triangle;

	uint32_t _render_target_count = 0;
	// TODO: The 8 should be "max bound targets" by graphics API.
	e_st_format _render_target_formats[8] = {};
	e_st_format _depth_stencil_format = st_format_unknown;

	st_sample_desc _sample_desc = {};

	// TODO: The rest of the D3D12_GRAPHICS_PIPELINE_STATE_DESC members.
};
