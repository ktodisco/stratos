#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/wglew.h>

GLenum topology_type_mappings[] =
{
	GL_POINTS, // st_primitive_topology_type_point
	GL_LINES, // st_primitive_topology_type_line
	GL_TRIANGLES, // st_primitive_topology_type_triangle
};

inline GLenum convert_topology_type(e_st_primitive_topology_type type)
{
	return topology_type_mappings[type];
}

GLenum topology_mappings[] =
{
	GL_POINTS, // st_primitive_topology_points
	GL_LINES, // st_primitive_topology_lines
	GL_TRIANGLES, // st_primitive_topology_triangles
};

inline GLenum convert_topology(e_st_primitive_topology topology)
{
	return topology_mappings[topology];
}

// HACK: In order to give a unique id to each format matching
// those of DXGI, several of which OGL has no equivalent for.
#define GL_UNKNOWN_FORMAT 0xf000000 | __LINE__

GLint format_mappings[] =
{
	GLU_UNKNOWN, // st_format_unknown
	GL_UNKNOWN_FORMAT, // st_format_r32g32b32a32_typeless
	GL_RGBA32F, // st_format_r32g32b32a32_float
	GL_RGBA32UI, // st_format_r32g32b32a32_uint
	GL_RGBA32I, // st_format_r32g32b32a32_sint
	GL_UNKNOWN_FORMAT, // st_format_r32g32b32_typeless
	GL_RGB32F, // st_format_r32g32b32_float
	GL_RGB32UI, // st_format_r32g32b32_uint
	GL_RGB32I, // st_format_r32g32b32_sint
	GL_UNKNOWN_FORMAT, // st_format_r16g16b16a16_typeless
	GL_RGBA16F, // st_format_r16g16b16a16_float
	GL_RGBA16, // st_format_r16g16b16a16_unorm
	GL_RGBA16UI, // st_format_r16g16b16a16_uint
	GL_RGBA16_SNORM, // st_format_r16g16b16a16_snorm
	GL_RGBA16I, // st_format_r16g16b16a16_sint
	GL_UNKNOWN_FORMAT, // st_format_r32g32_typeless
	GL_RG32F, // st_format_r32g32_float
	GL_RG32UI, // st_format_r32g32_uint
	GL_RG32I, // st_format_r32g32_sint
	GL_UNKNOWN_FORMAT, // st_format_r32g8x24_typeless
	GL_UNKNOWN_FORMAT, // st_format_d32_float_s8x24_uint
	GL_UNKNOWN_FORMAT, // st_format_r32_float_x8x24_typeless
	GL_UNKNOWN_FORMAT, // st_format_x32_typeless_g8x24_uint
	GL_UNKNOWN_FORMAT, // st_format_r10g10b10a2_typeless
	GL_RGB10_A2, // st_format_r10g10b10a2_unorm
	GL_RGB10_A2UI, // st_format_r10g10b10a2_uint
	GL_R11F_G11F_B10F, // st_format_r11g11b10_float
	GL_UNKNOWN_FORMAT, // st_format_r8g8b8a8_typeless
	GL_RGBA8, // st_format_r8g8b8a8_unorm
	GL_SRGB8_ALPHA8, // st_format_r8g8b8a8_unorm_srgb
	GL_RGBA8UI, // st_format_r8g8b8a8_uint
	GL_RGBA8_SNORM, // st_format_r8g8b8a8_snorm
	GL_RGBA8I, // st_format_r8g8b8a8_sint
	GL_UNKNOWN_FORMAT, // st_format_r16g16_typeless
	GL_RG16F, // st_format_r16g16_float
	GL_RG16, // st_format_r16g16_unorm
	GL_RG16UI, // st_format_r16g16_uint
	GL_RG16_SNORM, // st_format_r16g16_snorm
	GL_RG16I, // st_format_r16g16_sint
	GL_UNKNOWN_FORMAT, // st_format_r32_typeless
	GL_UNKNOWN_FORMAT, // st_format_d32_float
	GL_R32F, // st_format_r32_float
	GL_R32UI, // st_format_r32_uint
	GL_R32I, // st_format_r32_sint
	GL_UNKNOWN_FORMAT, // st_format_r24g8_typeless
	GL_DEPTH24_STENCIL8, // st_format_d24_unorm_s8_uint
	GL_UNKNOWN_FORMAT, // st_format_r24_unorm_x8_typeless
	GL_UNKNOWN_FORMAT, // st_format_x24_typeless_g8_uint
	GL_UNKNOWN_FORMAT, // st_format_r8g8_typeless
	GL_RG8, // st_format_r8g8_unorm
	GL_RG8UI, // st_format_r8g8_uint
	GL_RG8_SNORM, // st_format_r8g8_snorm
	GL_RG8I, // st_format_r8g8_sint
	GL_UNKNOWN_FORMAT, // st_format_r16_typeless
	GL_R16F, // st_format_r16_float
	GL_UNKNOWN_FORMAT, // st_format_d16_unorm
	GL_R16, // st_format_r16_unorm
	GL_R16UI, // st_format_r16_uint
	GL_R16_SNORM, // st_format_r16_snorm
	GL_R16I, // st_format_r16_sint
	GL_UNKNOWN_FORMAT, // st_format_r8_typeless
	GL_R8, // st_format_r8_unorm
	GL_R8UI, // st_format_r8_uint
	GL_R8_SNORM, // st_format_r8_snorm
	GL_R8I, // st_format_r8_sint
	GL_UNKNOWN_FORMAT, // st_format_a8_unorm
	GL_UNKNOWN_FORMAT, //st_format_r1_unorm
	GL_RGB9_E5, // st_format_r9g9b9e5_sharedexp
	GL_UNKNOWN_FORMAT, // st_format_r8g8_b8g8_unorm
	GL_UNKNOWN_FORMAT, // st_format_g8r8_g8b8_unorm
	GL_UNKNOWN_FORMAT, // st_format_bc1_typeless
	GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, // st_format_bc1_unorm
	GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, // st_format_bc1_unorm_srgb
	GL_UNKNOWN_FORMAT, // st_format_bc2_typeless
	GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, // st_format_bc2_unorm
	GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, // st_format_bc2_unorm_srgb
	GL_UNKNOWN_FORMAT, // st_format_bc3_typeless
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, // st_format_bc3_unorm
	GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, // st_format_bc3_unorm_srgb
	GL_UNKNOWN_FORMAT, // st_format_bc4_typeless
	GL_COMPRESSED_RED_RGTC1, // st_format_bc4_unorm
	GL_COMPRESSED_SIGNED_RED_RGTC1, // st_format_bc4_snorm
	GL_UNKNOWN_FORMAT, // st_format_bc5_typeless
	GL_COMPRESSED_RG_RGTC2, // st_format_bc5_unorm
	GL_COMPRESSED_SIGNED_RG_RGTC2, // st_format_bc5_snorm
	GL_UNKNOWN_FORMAT, // st_format_b5g6r5_unorm
	GL_UNKNOWN_FORMAT, // st_format_b5g5r5a1_unorm
	GL_UNKNOWN_FORMAT, // st_format_b8g8r8a8_unorm
	GL_UNKNOWN_FORMAT, // st_format_b8g8r8x8_unorm
	GL_UNKNOWN_FORMAT, // st_format_r10g10b10_xr_bias_a2_unorm
	GL_UNKNOWN_FORMAT, // st_format_b8g8r8a8_typeless
	GL_UNKNOWN_FORMAT, // st_format_b8g8r8a8_unorm_srgb
	GL_UNKNOWN_FORMAT, // st_format_b8g8r8x8_typeless
	GL_UNKNOWN_FORMAT, // st_format_b8g8r8x8_unorm_srgb
	GL_UNKNOWN_FORMAT, // st_format_bc6h_typeless
	GL_UNKNOWN_FORMAT, // st_format_bc6h_uf16
	GL_UNKNOWN_FORMAT, // st_format_bc6h_sf16
	GL_UNKNOWN_FORMAT, // st_format_bc7_typeless
	GL_COMPRESSED_RGBA_BPTC_UNORM, // st_format_bc7_unorm
	GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, // st_format_bc7_unorm_srgb
	GL_UNKNOWN_FORMAT, // st_format_ayuv
	GL_UNKNOWN_FORMAT, // st_format_y410
	GL_UNKNOWN_FORMAT, // st_format_y416
	GL_UNKNOWN_FORMAT, // st_format_nv12
	GL_UNKNOWN_FORMAT, // st_format_p010
	GL_UNKNOWN_FORMAT, // st_format_p016
	GL_UNKNOWN_FORMAT, // st_format_420_opaque
	GL_UNKNOWN_FORMAT, // st_format_yuy2
	GL_UNKNOWN_FORMAT, // st_format_y210
	GL_UNKNOWN_FORMAT, // st_format_y216
	GL_UNKNOWN_FORMAT, // st_format_nv11
	GL_UNKNOWN_FORMAT, // st_format_ai44
	GL_UNKNOWN_FORMAT, // st_format_ia44
	GL_UNKNOWN_FORMAT, // st_format_p8
	GL_UNKNOWN_FORMAT, // st_format_a8p8
	GL_UNKNOWN_FORMAT, // st_format_b4g4r4a4_unorm
	GL_UNKNOWN_FORMAT, // st_format_p208
	GL_UNKNOWN_FORMAT, // st_format_v208
	GL_UNKNOWN_FORMAT, // st_format_v408
	GL_UNKNOWN_FORMAT, // st_format_force_uint
};

inline GLint convert_format(e_st_format format)
{
	return format_mappings[format];
}

GLenum blend_mappings[] =
{
	GL_ZERO, // st_blend_zero
	GL_ONE, // st_blend_one
	GL_SRC_COLOR, // st_blend_src_color
	GL_ONE_MINUS_SRC_COLOR, // st_blend_inv_src_color
	GL_SRC_ALPHA, // st_blend_src_alpha
	GL_ONE_MINUS_SRC_ALPHA, // st_blend_inv_src_alpha
	GL_DST_ALPHA, // st_blend_dst_alpha
	GL_ONE_MINUS_DST_ALPHA, // st_blend_inv_dst_alpha
	GL_DST_COLOR, // st_blend_dst_color
	GL_ONE_MINUS_DST_COLOR, // st_blend_inv_dst_color
	GL_SRC_ALPHA_SATURATE, // st_blend_src_alpha_sat
	GL_CONSTANT_COLOR, // st_blend_blend_factor
	GL_ONE_MINUS_CONSTANT_COLOR, // st_blend_inv_blend_factor
	GL_SRC1_COLOR, // st_blend_src1_color
	GL_ONE_MINUS_SRC1_COLOR, // st_blend_inv_src1_color
	GL_SRC1_ALPHA, // st_blend_src1_alpha
	GL_ONE_MINUS_SRC1_ALPHA, // st_blend_inv_src1_alpha
};

inline GLenum convert_blend(e_st_blend blend)
{
	return blend_mappings[blend];
}

GLenum blend_op_mappings[] =
{
	GL_FUNC_ADD, // st_blend_op_add
	GL_FUNC_SUBTRACT, // st_blend_op_sub
	GL_FUNC_REVERSE_SUBTRACT, // st_blend_op_rev_sub
	GL_MIN, // st_blend_op_min
	GL_MAX, // st_blend_op_max
};

inline GLenum convert_blend_op(e_st_blend_op blend_op)
{
	return blend_op_mappings[blend_op];
}

GLenum logic_op_mappings[] =
{
	GL_CLEAR, // st_logic_op_clear
	GL_SET, // st_logic_op_set
	GL_COPY, // st_logic_op_copy
	GL_COPY_INVERTED, // st_logic_op_copy_inverted
	GL_NOOP, // st_logic_op_noop
	GL_INVERT, // st_logic_op_invert
	GL_AND, // st_logic_op_and
	GL_NAND, // st_logic_op_nand
	GL_OR, // st_logic_op_or
	GL_NOR, // st_logic_op_nor
	GL_XOR, // st_logic_op_xor
	GL_EQUIV, // st_logic_op_equiv
	GL_AND_REVERSE, // st_logic_op_and_reverse
	GL_AND_INVERTED, // st_logic_op_and_inverted
	GL_OR_REVERSE, // st_logic_op_or_reverse
	GL_OR_INVERTED, // st_logic_op_or_inverted
};

inline GLenum convert_logic_op(e_st_logic_op logic_op)
{
	return logic_op_mappings[logic_op];
}

GLenum fill_mode_mappings[] =
{
	GL_LINE, // st_fill_mode_wireframe
	GL_FILL, // st_fill_mode_solid
};

inline GLenum convert_fill_mode(e_st_fill_mode fill_mode)
{
	return fill_mode_mappings[fill_mode];
}

GLenum cull_mode_mappings[] =
{
	GL_NONE, // st_cull_mode_none - this is an invalid enum for glCullFace intentionally.
	GL_FRONT, // st_cull_mode_front
	GL_BACK, // st_cull_mode_back
};

inline GLenum convert_cull_mode(e_st_cull_mode cull_mode)
{
	return cull_mode_mappings[cull_mode];
}

GLenum depth_write_mask_mappings[] =
{
	GL_ZERO, // st_depth_write_mask_zero
	GL_ONE, // st_depth_write_mask_all
};

inline GLenum convert_depth_write_mask(e_st_depth_write_mask write_mask)
{
	return depth_write_mask_mappings[write_mask];
}

GLenum compare_func_mappings[] =
{
	GL_NEVER, // st_compare_func_never
	GL_LESS, // st_compare_func_less
	GL_EQUAL, // st_compare_func_equal
	GL_LEQUAL, // st_compare_func_less_equal
	GL_GREATER, // st_compare_func_greater
	GL_NOTEQUAL, // st_compare_func_not_equal
	GL_GEQUAL, // st_compare_func_greater_equal
	GL_ALWAYS, // st_compare_func_always
};

inline GLenum convert_compare_func(e_st_compare_func compare_func)
{
	return compare_func_mappings[compare_func];
}

GLenum stencil_op_mappings[] =
{
	GL_KEEP, // st_stencil_op_keep
	GL_ZERO, // st_stencil_op_zero
	GL_REPLACE, // st_stencil_op_replace
	GL_INCR, // st_stencil_op_incr_sat
	GL_DECR, // st_stencil_op_decr_sat
	GL_INVERT, // st_stencil_op_invert
	GL_INCR_WRAP, // st_stencil_op_incr
	GL_DECR_WRAP, // st_stencil_op_decr
};

inline GLenum convert_stencil_op(e_st_stencil_op stencil_op)
{
	return stencil_op_mappings[stencil_op];
}

GLenum address_mode_mappings[] =
{
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER,
};

inline GLenum convert_address_mode(e_st_address_mode mode)
{
	return address_mode_mappings[mode];
}

#endif
