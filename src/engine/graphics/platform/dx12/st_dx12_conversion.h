#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <d3d12.h>
#include <dxgi1_6.h>

D3D12_PRIMITIVE_TOPOLOGY_TYPE topology_type_mappings[] =
{
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, // st_primitive_topology_type_point
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, // st_primitive_topology_type_line
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, //st_primitive_topology_type_triangle
};

inline D3D12_PRIMITIVE_TOPOLOGY_TYPE convert_topology_type(e_st_primitive_topology_type type)
{
	return topology_type_mappings[type];
}

D3D12_PRIMITIVE_TOPOLOGY topology_mappings[] =
{
	D3D_PRIMITIVE_TOPOLOGY_POINTLIST, // st_primitive_topology_points
	D3D_PRIMITIVE_TOPOLOGY_LINELIST, // st_primitive_topology_lines
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, // st_primitive_topology_triangles
};

inline D3D12_PRIMITIVE_TOPOLOGY convert_topology(e_st_primitive_topology topology)
{
	return topology_mappings[topology];
}

DXGI_FORMAT format_mappings[] =
{
	DXGI_FORMAT_UNKNOWN, // st_format_unknown
	DXGI_FORMAT_R32G32B32A32_TYPELESS, // st_format_r32g32b32a32_typeless
	DXGI_FORMAT_R32G32B32A32_FLOAT, // st_format_r32g32b32a32_float
	DXGI_FORMAT_R32G32B32A32_UINT, // st_format_r32g32b32a32_uint
	DXGI_FORMAT_R32G32B32A32_SINT, // st_format_r32g32b32a32_sint
	DXGI_FORMAT_R32G32B32_TYPELESS, // st_format_r32g32b32_typeless
	DXGI_FORMAT_R32G32B32_FLOAT, // st_format_r32g32b32_float
	DXGI_FORMAT_R32G32B32_UINT, // st_format_r32g32b32_uint
	DXGI_FORMAT_R32G32B32_SINT, // st_format_r32g32b32_sint
	DXGI_FORMAT_R16G16B16A16_TYPELESS, // st_format_r16g16b16a16_typeless
	DXGI_FORMAT_R16G16B16A16_FLOAT, // st_format_r16g16b16a16_float
	DXGI_FORMAT_R16G16B16A16_UNORM, // st_format_r16g16b16a16_unorm
	DXGI_FORMAT_R16G16B16A16_UINT, // st_format_r16g16b16a16_uint
	DXGI_FORMAT_R16G16B16A16_SNORM, // st_format_r16g16b16a16_snorm
	DXGI_FORMAT_R16G16B16A16_SINT, // st_format_r16g16b16a16_sint
	DXGI_FORMAT_R32G32_TYPELESS, // st_format_r32g32_typeless
	DXGI_FORMAT_R32G32_FLOAT, // st_format_r32g32_float
	DXGI_FORMAT_R32G32_UINT, // st_format_r32g32_uint
	DXGI_FORMAT_R32G32_SINT, // st_format_r32g32_sint
	DXGI_FORMAT_R32G8X24_TYPELESS, // st_format_r32g8x24_typeless
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT, // st_format_d32_float_s8x24_uint
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, // st_format_r32_float_x8x24_typeless
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT, // st_format_x32_typeless_g8x24_uint
	DXGI_FORMAT_R10G10B10A2_TYPELESS, // st_format_r10g10b10a2_typeless
	DXGI_FORMAT_R10G10B10A2_UNORM, // st_format_r10g10b10a2_unorm
	DXGI_FORMAT_R10G10B10A2_UINT, // st_format_r10g10b10a2_uint
	DXGI_FORMAT_R11G11B10_FLOAT, // st_format_r11g11b10_float
	DXGI_FORMAT_R8G8B8A8_TYPELESS, // st_format_r8g8b8a8_typeless
	DXGI_FORMAT_R8G8B8A8_UNORM, // st_format_r8g8b8a8_unorm
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // st_format_r8g8b8a8_unorm_srgb
	DXGI_FORMAT_R8G8B8A8_UINT, // st_format_r8g8b8a8_uint
	DXGI_FORMAT_R8G8B8A8_SNORM, // st_format_r8g8b8a8_snorm
	DXGI_FORMAT_R8G8B8A8_SINT, // st_format_r8g8b8a8_sint
	DXGI_FORMAT_R16G16_TYPELESS, // st_format_r16g16_typeless
	DXGI_FORMAT_R16G16_FLOAT, // st_format_r16g16_float
	DXGI_FORMAT_R16G16_UNORM, // st_format_r16g16_unorm
	DXGI_FORMAT_R16G16_UINT, // st_format_r16g16_uint
	DXGI_FORMAT_R16G16_SNORM, // st_format_r16g16_snorm
	DXGI_FORMAT_R16G16_SINT, // st_format_r16g16_sint
	DXGI_FORMAT_R32_TYPELESS, // st_format_r32_typeless
	DXGI_FORMAT_D32_FLOAT, // st_format_d32_float
	DXGI_FORMAT_R32_FLOAT, // st_format_r32_float
	DXGI_FORMAT_R32_UINT, // st_format_r32_uint
	DXGI_FORMAT_R32_SINT, // st_format_r32_sint
	DXGI_FORMAT_R24G8_TYPELESS, // st_format_r24g8_typeless
	DXGI_FORMAT_D24_UNORM_S8_UINT, // st_format_d24_unorm_s8_uint
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS, // st_format_r24_unorm_x8_typeless
	DXGI_FORMAT_X24_TYPELESS_G8_UINT, // st_format_x24_typeless_g8_uint
	DXGI_FORMAT_R8G8_TYPELESS, // st_format_r8g8_typeless
	DXGI_FORMAT_R8G8_UNORM, // st_format_r8g8_unorm
	DXGI_FORMAT_R8G8_UINT, // st_format_r8g8_uint
	DXGI_FORMAT_R8G8_SNORM, // st_format_r8g8_snorm
	DXGI_FORMAT_R8G8_SINT, // st_format_r8g8_sint
	DXGI_FORMAT_R16_TYPELESS, // st_format_r16_typeless
	DXGI_FORMAT_R16_FLOAT, // st_format_r16_float
	DXGI_FORMAT_D16_UNORM, // st_format_d16_unorm
	DXGI_FORMAT_R16_UNORM, // st_format_r16_unorm
	DXGI_FORMAT_R16_UINT, // st_format_r16_uint
	DXGI_FORMAT_R16_SNORM, // st_format_r16_snorm
	DXGI_FORMAT_R16_SINT, // st_format_r16_sint
	DXGI_FORMAT_R8_TYPELESS, // st_format_r8_typeless
	DXGI_FORMAT_R8_UNORM, // st_format_r8_unorm
	DXGI_FORMAT_R8_UINT, // st_format_r8_uint
	DXGI_FORMAT_R8_SNORM, // st_format_r8_snorm
	DXGI_FORMAT_R8_SINT, // st_format_r8_sint
	DXGI_FORMAT_A8_UNORM, // st_format_a8_unorm
	DXGI_FORMAT_R1_UNORM, // st_format_r1_unorm
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP, // st_format_r9g9b9e5_sharedexp
	DXGI_FORMAT_R8G8_B8G8_UNORM, // st_format_r8g8_b8g8_unorm
	DXGI_FORMAT_G8R8_G8B8_UNORM, // st_format_g8r8_g8b8_unorm
	DXGI_FORMAT_BC1_TYPELESS, // st_format_bc1_typeless
	DXGI_FORMAT_BC1_UNORM, // st_format_bc1_unorm
	DXGI_FORMAT_BC1_UNORM_SRGB, // st_format_bc1_unorm_srgb
	DXGI_FORMAT_BC2_TYPELESS, // st_format_bc2_typeless
	DXGI_FORMAT_BC2_UNORM, // st_format_bc2_unorm
	DXGI_FORMAT_BC2_UNORM_SRGB, // st_format_bc2_unorm_srgb
	DXGI_FORMAT_BC3_TYPELESS, // st_format_bc3_typeless
	DXGI_FORMAT_BC3_UNORM, // st_format_bc3_unorm
	DXGI_FORMAT_BC3_UNORM_SRGB, // st_format_bc3_unorm_srgb
	DXGI_FORMAT_BC4_TYPELESS, // st_format_bc4_typeless
	DXGI_FORMAT_BC4_UNORM, // st_format_bc4_unorm
	DXGI_FORMAT_BC4_SNORM, // st_format_bc4_snorm
	DXGI_FORMAT_BC5_TYPELESS, // st_format_bc5_typeless
	DXGI_FORMAT_BC5_UNORM, // st_format_bc5_unorm
	DXGI_FORMAT_BC5_SNORM, // st_format_bc5_snorm
	DXGI_FORMAT_B5G6R5_UNORM, // st_format_b5g6r5_unorm
	DXGI_FORMAT_B5G5R5A1_UNORM, // st_format_b5g5r5a1_unorm
	DXGI_FORMAT_B8G8R8A8_UNORM, // st_format_b8g8r8a8_unorm
	DXGI_FORMAT_B8G8R8X8_UNORM, // st_format_b8g8r8x8_unorm
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, // st_format_r10g10b10_xr_bias_a2_unorm
	DXGI_FORMAT_B8G8R8A8_TYPELESS, // st_format_b8g8r8a8_typeless
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, // st_format_b8g8r8a8_unorm_srgb
	DXGI_FORMAT_B8G8R8X8_TYPELESS, // st_format_b8g8r8x8_typeless
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB, // st_format_b8g8r8x8_unorm_srgb
	DXGI_FORMAT_BC6H_TYPELESS, // st_format_bc6h_typeless
	DXGI_FORMAT_BC6H_UF16, // st_format_bc6h_uf16
	DXGI_FORMAT_BC6H_SF16, // st_format_bc6h_sf16
	DXGI_FORMAT_BC7_TYPELESS, // st_format_bc7_typeless
	DXGI_FORMAT_BC7_UNORM, // st_format_bc7_unorm
	DXGI_FORMAT_BC7_UNORM_SRGB, // st_format_bc7_unorm_srgb
	DXGI_FORMAT_AYUV, // st_format_ayuv
	DXGI_FORMAT_Y410, // st_format_y410
	DXGI_FORMAT_Y416, // st_format_y416
	DXGI_FORMAT_NV12, // st_format_nv12
	DXGI_FORMAT_P010, // st_format_p010
	DXGI_FORMAT_P016, // st_format_p016
	DXGI_FORMAT_420_OPAQUE, // st_format_420_opaque
	DXGI_FORMAT_YUY2, // st_format_yuy2
	DXGI_FORMAT_Y210, // st_format_y210
	DXGI_FORMAT_Y216, // st_format_y216
	DXGI_FORMAT_NV11, // st_format_nv11
	DXGI_FORMAT_AI44, // st_format_ai44
	DXGI_FORMAT_IA44, // st_format_ia44
	DXGI_FORMAT_P8, // st_format_p8
	DXGI_FORMAT_A8P8, // st_format_a8p8
	DXGI_FORMAT_B4G4R4A4_UNORM, // st_format_b4g4r4a4_unorm
	DXGI_FORMAT_P208, // st_format_p208
	DXGI_FORMAT_V208, // st_format_v208
	DXGI_FORMAT_V408, // st_format_v408
	DXGI_FORMAT_FORCE_UINT, // st_format_force_uint
};

inline DXGI_FORMAT convert_format(e_st_format format)
{
	return format_mappings[format];
}

D3D12_BLEND blend_mappings[] =
{
	D3D12_BLEND_ZERO, // st_blend_zero
	D3D12_BLEND_ONE, // st_blend_one
	D3D12_BLEND_SRC_COLOR, // st_blend_src_color
	D3D12_BLEND_INV_SRC_COLOR, // st_blend_inv_src_color
	D3D12_BLEND_SRC_ALPHA, // st_blend_src_alpha
	D3D12_BLEND_INV_SRC_ALPHA, // st_blend_inv_src_alpha
	D3D12_BLEND_DEST_ALPHA, // st_blend_dst_alpha
	D3D12_BLEND_INV_DEST_ALPHA, // st_blend_inv_dst_alpha
	D3D12_BLEND_DEST_COLOR, // st_blend_dst_color
	D3D12_BLEND_INV_DEST_COLOR, // st_blend_inv_dst_color
	D3D12_BLEND_SRC_ALPHA_SAT, // st_blend_src_alpha_sat
	D3D12_BLEND_BLEND_FACTOR, // st_blend_blend_factor
	D3D12_BLEND_INV_BLEND_FACTOR, // st_blend_inv_blend_factor
	D3D12_BLEND_SRC1_COLOR, // st_blend_src1_color
	D3D12_BLEND_INV_SRC1_COLOR, // st_blend_inv_src1_color
	D3D12_BLEND_SRC1_ALPHA, // st_blend_src1_alpha
	D3D12_BLEND_INV_SRC1_ALPHA, // st_blend_inv_src1_alpha
};

inline D3D12_BLEND convert_blend(e_st_blend blend)
{
	return blend_mappings[blend];
}

D3D12_BLEND_OP blend_op_mappings[] =
{
	D3D12_BLEND_OP_ADD, // st_blend_op_add
	D3D12_BLEND_OP_SUBTRACT, // st_blend_op_sub
	D3D12_BLEND_OP_REV_SUBTRACT, // st_blend_op_rev_sub
	D3D12_BLEND_OP_MIN, // st_blend_op_min
	D3D12_BLEND_OP_MAX, // st_blend_op_max
};

inline D3D12_BLEND_OP convert_blend_op(e_st_blend_op blend_op)
{
	return blend_op_mappings[blend_op];
}

D3D12_LOGIC_OP logic_op_mappings[] =
{
	D3D12_LOGIC_OP_CLEAR, // st_logic_op_clear
	D3D12_LOGIC_OP_SET, // st_logic_op_set
	D3D12_LOGIC_OP_COPY, // st_logic_op_copy
	D3D12_LOGIC_OP_COPY_INVERTED, // st_logic_op_copy_inverted
	D3D12_LOGIC_OP_NOOP, // st_logic_op_noop
	D3D12_LOGIC_OP_INVERT, // st_logic_op_invert
	D3D12_LOGIC_OP_AND, // st_logic_op_and
	D3D12_LOGIC_OP_NAND, // st_logic_op_nand
	D3D12_LOGIC_OP_OR, // st_logic_op_or
	D3D12_LOGIC_OP_NOR, // st_logic_op_nor
	D3D12_LOGIC_OP_XOR, // st_logic_op_xor
	D3D12_LOGIC_OP_EQUIV, // st_logic_op_equiv
	D3D12_LOGIC_OP_AND_REVERSE, // st_logic_op_and_reverse
	D3D12_LOGIC_OP_AND_INVERTED, // st_logic_op_and_inverted
	D3D12_LOGIC_OP_OR_REVERSE, // st_logic_op_or_reverse
	D3D12_LOGIC_OP_OR_INVERTED, // st_logic_op_or_inverted
};

inline D3D12_LOGIC_OP convert_logic_op(e_st_logic_op logic_op)
{
	return logic_op_mappings[logic_op];
}

D3D12_FILL_MODE fill_mode_mappings[] =
{
	D3D12_FILL_MODE_WIREFRAME, // st_fill_mode_wireframe
	D3D12_FILL_MODE_SOLID, // st_fill_mode_solid
};

inline D3D12_FILL_MODE convert_fill_mode(e_st_fill_mode fill_mode)
{
	return fill_mode_mappings[fill_mode];
}

D3D12_CULL_MODE cull_mode_mappings[] =
{
	D3D12_CULL_MODE_NONE, // st_cull_mode_none
	D3D12_CULL_MODE_FRONT, // st_cull_mode_front
	D3D12_CULL_MODE_BACK, // st_cull_mode_back
};

inline D3D12_CULL_MODE convert_cull_mode(e_st_cull_mode cull_mode)
{
	return cull_mode_mappings[cull_mode];
}

D3D12_DEPTH_WRITE_MASK depth_write_mask_mappings[] =
{
	D3D12_DEPTH_WRITE_MASK_ZERO, // st_depth_write_mask_zero
	D3D12_DEPTH_WRITE_MASK_ALL, // st_depth_write_mask_all
};

inline D3D12_DEPTH_WRITE_MASK convert_depth_write_mask(e_st_depth_write_mask write_mask)
{
	return depth_write_mask_mappings[write_mask];
}

D3D12_COMPARISON_FUNC comparison_func_mappings[] =
{
	D3D12_COMPARISON_FUNC_NEVER, // st_compare_func_never
	D3D12_COMPARISON_FUNC_LESS, // st_compare_func_less
	D3D12_COMPARISON_FUNC_EQUAL, // st_compare_func_equal
	D3D12_COMPARISON_FUNC_LESS_EQUAL, // st_compare_func_less_equal
	D3D12_COMPARISON_FUNC_GREATER, // st_compare_func_greater
	D3D12_COMPARISON_FUNC_NOT_EQUAL, // st_compare_func_not_equal
	D3D12_COMPARISON_FUNC_GREATER_EQUAL, // st_compare_func_greater_equal
	D3D12_COMPARISON_FUNC_ALWAYS, // st_compare_func_always
};

inline D3D12_COMPARISON_FUNC convert_comparison_func(e_st_compare_func compare_func)
{
	return comparison_func_mappings[compare_func];
}

D3D12_STENCIL_OP stencil_op_mappings[] =
{
	D3D12_STENCIL_OP_KEEP, // st_stencil_op_keep
	D3D12_STENCIL_OP_ZERO, // st_stencil_op_zero
	D3D12_STENCIL_OP_REPLACE, // st_stencil_op_replace
	D3D12_STENCIL_OP_INCR_SAT, // st_stencil_op_incr_sat
	D3D12_STENCIL_OP_DECR_SAT, // st_stencil_op_decr_sat
	D3D12_STENCIL_OP_INVERT, // st_stencil_op_invert
	D3D12_STENCIL_OP_INCR, // st_stencil_op_incr
	D3D12_STENCIL_OP_DECR, // st_stencil_op_decr
};

inline D3D12_STENCIL_OP convert_stencil_op(e_st_stencil_op stencil_op)
{
	return stencil_op_mappings[stencil_op];
}

D3D12_RESOURCE_STATES resource_state_mappings[] =
{
	D3D12_RESOURCE_STATE_COMMON, // st_texture_state_common
	D3D12_RESOURCE_STATE_RENDER_TARGET, // st_texture_state_render_target
	D3D12_RESOURCE_STATE_RENDER_TARGET, // st_texture_state_depth_stencil_target
	D3D12_RESOURCE_STATE_DEPTH_WRITE, // st_texture_state_depth_target
	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, // st_texture_state_non_pixel_shader_read
	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // st_texture_state_pixel_shader_read
	D3D12_RESOURCE_STATE_DEPTH_READ, // st_texture_state_depth_read
	D3D12_RESOURCE_STATE_PRESENT, // st_texture_state_present
	D3D12_RESOURCE_STATE_COPY_SOURCE, // st_texture_state_copy_source
	D3D12_RESOURCE_STATE_COPY_DEST, // st_texture_state_copy_dest
};

inline D3D12_RESOURCE_STATES convert_resource_state(e_st_texture_state state)
{
	return resource_state_mappings[state];
}

D3D12_TEXTURE_ADDRESS_MODE address_mode_mappings[] =
{
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	D3D12_TEXTURE_ADDRESS_MODE_BORDER,
};

inline D3D12_TEXTURE_ADDRESS_MODE convert_address_mode(e_st_address_mode mode)
{
	return address_mode_mappings[mode];
}

#endif
