#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <cstdint>

#include <Windows.h>
#include <wrl.h>

enum e_st_blend
{
	st_blend_zero				= D3D12_BLEND_ZERO,
	st_blend_one				= D3D12_BLEND_ONE,
	st_blend_src_color			= D3D12_BLEND_SRC_COLOR,
	st_blend_inv_src_color		= D3D12_BLEND_INV_SRC_COLOR,
	st_blend_src_alpha			= D3D12_BLEND_SRC_ALPHA,
	st_blend_inv_src_alpha		= D3D12_BLEND_INV_SRC_ALPHA,
	st_blend_dst_alpha			= D3D12_BLEND_DEST_ALPHA,
	st_blend_inv_dst_alpha		= D3D12_BLEND_INV_DEST_ALPHA,
	st_blend_dst_color			= D3D12_BLEND_DEST_COLOR,
	st_blend_inv_dst_color		= D3D12_BLEND_INV_DEST_COLOR,
	st_blend_src_alpha_sat		= D3D12_BLEND_SRC_ALPHA_SAT,
	st_blend_blend_factor		= D3D12_BLEND_BLEND_FACTOR,
	st_blend_inv_blend_factor	= D3D12_BLEND_INV_BLEND_FACTOR,
	st_blend_src1_color			= D3D12_BLEND_SRC1_COLOR,
	st_blend_inv_src1_color		= D3D12_BLEND_INV_SRC1_COLOR,
	st_blend_src1_alpha			= D3D12_BLEND_SRC1_ALPHA,
	st_blend_inv_src1_alpha		= D3D12_BLEND_INV_SRC1_ALPHA,
};

enum e_st_blend_op
{
	st_blend_op_add				= D3D12_BLEND_OP_ADD,
	st_blend_op_sub				= D3D12_BLEND_OP_SUBTRACT,
	st_blend_op_rev_sub			= D3D12_BLEND_OP_REV_SUBTRACT,
	st_blend_op_min				= D3D12_BLEND_OP_MIN,
	st_blend_op_max				= D3D12_BLEND_OP_MAX,
};

enum e_st_logic_op
{
	st_logic_op_clear			= D3D12_LOGIC_OP_CLEAR,
	st_logic_op_set				= D3D12_LOGIC_OP_SET,
	st_logic_op_copy			= D3D12_LOGIC_OP_COPY,
	st_logic_op_copy_inverted	= D3D12_LOGIC_OP_COPY_INVERTED,
	st_logic_op_noop			= D3D12_LOGIC_OP_NOOP,
	st_logic_op_invert			= D3D12_LOGIC_OP_INVERT,
	st_logic_op_and				= D3D12_LOGIC_OP_AND,
	st_logic_op_nand			= D3D12_LOGIC_OP_NAND,
	st_logic_op_or				= D3D12_LOGIC_OP_OR,
	st_logic_op_nor				= D3D12_LOGIC_OP_NOR,
	st_logic_op_xor				= D3D12_LOGIC_OP_XOR,
	st_logic_op_equiv			= D3D12_LOGIC_OP_EQUIV,
	st_logic_op_and_reverse		= D3D12_LOGIC_OP_AND_REVERSE,
	st_logic_op_and_inverted	= D3D12_LOGIC_OP_AND_INVERTED,
	st_logic_op_or_reverse		= D3D12_LOGIC_OP_OR_REVERSE,
	st_logic_op_or_inverted		= D3D12_LOGIC_OP_OR_INVERTED,
};

struct st_dx12_render_target_blend_desc
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

	// TODO: Logic ops.
	e_st_logic_op _logic_op = st_logic_op_noop;
};

struct st_dx12_pipeline_blend_desc
{
	bool _alpha_to_coverage = false;
	bool _independent_blend = false;

	st_dx12_render_target_blend_desc _target_blend[8] = {};
};

enum e_st_fill_mode
{
	st_fill_mode_wireframe	= D3D12_FILL_MODE_WIREFRAME,
	st_fill_mode_solid		= D3D12_FILL_MODE_SOLID,
};

enum e_st_cull_mode
{
	st_cull_mode_none		= D3D12_CULL_MODE_NONE,
	st_cull_mode_front		= D3D12_CULL_MODE_FRONT,
	st_cull_mode_back		= D3D12_CULL_MODE_BACK,
};

struct st_dx12_pipeline_rasterizer_desc
{
	e_st_fill_mode _fill_mode = st_fill_mode_solid;
	e_st_cull_mode _cull_mode = st_cull_mode_back;

	bool _winding_order_clockwise = false;

	// TODO: Rest of D3D12_RASTERIZER_DESC.
};

enum e_st_depth_write_mask
{
	st_depth_write_mask_zero		= D3D12_DEPTH_WRITE_MASK_ZERO,
	st_depth_write_mask_all			= D3D12_DEPTH_WRITE_MASK_ALL,
};

enum e_st_compare_func
{
	st_compare_func_never			= D3D12_COMPARISON_FUNC_NEVER,
	st_compare_func_less			= D3D12_COMPARISON_FUNC_LESS,
	st_compare_func_equal			= D3D12_COMPARISON_FUNC_EQUAL,
	st_compare_func_less_equal		= D3D12_COMPARISON_FUNC_LESS_EQUAL,
	st_compare_func_greater			= D3D12_COMPARISON_FUNC_GREATER,
	st_compare_func_not_equal		= D3D12_COMPARISON_FUNC_NOT_EQUAL,
	st_compare_func_greater_equal	= D3D12_COMPARISON_FUNC_GREATER_EQUAL,
	st_compare_func_always			= D3D12_COMPARISON_FUNC_ALWAYS,
};

enum e_st_stencil_op
{
	st_stencil_op_keep			= D3D12_STENCIL_OP_KEEP,
	st_stencil_op_zero			= D3D12_STENCIL_OP_ZERO,
	st_stencil_op_replace		= D3D12_STENCIL_OP_REPLACE,
	st_stencil_op_incr_sat		= D3D12_STENCIL_OP_INCR_SAT,
	st_stencil_op_decr_sat		= D3D12_STENCIL_OP_DECR_SAT,
	st_stencil_op_invert		= D3D12_STENCIL_OP_INVERT,
	st_stencil_op_incr			= D3D12_STENCIL_OP_INCR,
	st_stencil_op_decr			= D3D12_STENCIL_OP_DECR,
};

struct st_dx12_pipeline_stencil_op_desc
{
	e_st_stencil_op _stencil_fail_op = st_stencil_op_keep;
	e_st_stencil_op _depth_fail_op = st_stencil_op_keep;
	e_st_stencil_op _stencil_pass_op = st_stencil_op_keep;
	e_st_compare_func _stencil_func = st_compare_func_less;
};

struct st_dx12_pipeline_depth_stencil_desc
{
	bool _depth_enable = false;

	e_st_depth_write_mask _depth_mask = st_depth_write_mask_all;
	e_st_compare_func _depth_compare = st_compare_func_less;

	bool _stencil_enable = false;
	uint8_t _stencil_read_mask = 0;
	uint8_t _stencil_write_mask = 0;

	st_dx12_pipeline_stencil_op_desc _front_stencil = {};
	st_dx12_pipeline_stencil_op_desc _back_stencil = {};
};

struct st_dx12_sample_desc
{
	uint32_t _count = 1;
	uint32_t _quality = 0;
};

struct st_dx12_pipeline_state_desc
{
	const class st_shader* _shader = {};

	uint32_t _sample_mask = UINT_MAX;

	st_dx12_pipeline_blend_desc _blend_desc = {};
	st_dx12_pipeline_rasterizer_desc _rasterizer_desc = {};
	st_dx12_pipeline_depth_stencil_desc _depth_stencil_desc = {};

	class st_dx12_vertex_format* _vertex_format = nullptr;

	e_st_primitive_topology_type _primitive_topology_type = st_primitive_topology_type_triangle;

	uint32_t _render_target_count = 0;
	// TODO: The 8 should be "max bound targets" by graphics API.
	e_st_texture_format _render_target_formats[8] = {};
	e_st_texture_format _depth_stencil_format = st_texture_format_unknown;

	st_dx12_sample_desc _sample_desc = {};

	// TODO: The rest of the D3D12_GRAPHICS_PIPELINE_STATE_DESC members.
};

class st_dx12_pipeline_state
{
public:
	st_dx12_pipeline_state(const st_dx12_pipeline_state_desc& desc);
	~st_dx12_pipeline_state();

	ID3D12PipelineState* get_state() const { return _pipeline_state.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipeline_state;
};
