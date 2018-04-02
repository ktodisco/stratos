#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <pix3.h>

#include <graphics/dx12/d3dx12.h>

#include <Windows.h>
#include <wrl.h>

#if defined(_DEBUG)
#define ST_NAME_DX12_OBJECT(object, name) (object)->SetName((LPCWSTR)name)
#else
#define ST_NAME_DX12_OBJECT(object, name)
#endif

enum e_st_primitive_topology_type
{
	st_primitive_topology_type_point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
	st_primitive_topology_type_line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
	st_primitive_topology_type_triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
};

enum e_st_primitive_topology
{
	st_primitive_topology_points = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
	st_primitive_topology_lines = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
	st_primitive_topology_triangles = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
};

enum e_st_texture_format
{
	st_texture_format_unknown = DXGI_FORMAT_UNKNOWN,
	st_texture_format_r8_unorm = DXGI_FORMAT_R8_UNORM,
	st_texture_format_r8_uint = DXGI_FORMAT_R8_UINT,
	st_texture_format_r8g8b8a8_snorm = DXGI_FORMAT_R8G8B8A8_SNORM,
	st_texture_format_r8g8b8a8_unorm = DXGI_FORMAT_R8G8B8A8_UNORM,
	st_texture_format_r8g8b8a8_uint = DXGI_FORMAT_R8G8B8A8_UINT,
	st_texture_format_r24_unorm_x8_typeless = DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
	st_texture_format_d24_unorm_s8_uint = DXGI_FORMAT_D24_UNORM_S8_UINT,
};

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
