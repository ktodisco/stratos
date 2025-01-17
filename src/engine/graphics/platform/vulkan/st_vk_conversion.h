#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

static vk::PrimitiveTopology topology_type_mappings[] =
{
	vk::PrimitiveTopology::ePointList, // st_primitive_topology_type_point
	vk::PrimitiveTopology::eLineList, // st_primitive_topology_type_line
	vk::PrimitiveTopology::eTriangleList, // st_primitive_topology_type_triangle
};

inline vk::PrimitiveTopology convert_topology_type(e_st_primitive_topology_type type)
{
	return topology_type_mappings[type];
}

static vk::PrimitiveTopology topology_mappings[] =
{
	vk::PrimitiveTopology::ePointList, // st_primitive_topology_points
	vk::PrimitiveTopology::eLineList, // st_primitive_topology_lines
	vk::PrimitiveTopology::eTriangleList, // st_primitive_topology_triangles
};

inline vk::PrimitiveTopology convert_topology(e_st_primitive_topology topology)
{
	return topology_mappings[topology];
}

// HACK: In order to give a unique id to each format matching
// those of DXGI, several of which vulkan has no equivalent for.
#define VK_UNKNOWN_FORMAT 0xf000000 | __LINE__

static vk::Format format_mappings[] =
{
	vk::Format::eUndefined, // st_format_unknown
	vk::Format::eUndefined, // st_format_r32g32b32a32_typeless
	vk::Format::eR32G32B32A32Sfloat, // st_format_r32g32b32a32_float
	vk::Format::eR32G32B32A32Uint, // st_format_r32g32b32a32_uint
	vk::Format::eR32G32B32A32Sint, // st_format_r32g32b32a32_sint
	vk::Format::eUndefined, // st_format_r32g32b32_typeless
	vk::Format::eR32G32B32Sfloat, // st_format_r32g32b32_float
	vk::Format::eR32G32B32Uint, // st_format_r32g32b32_uint
	vk::Format::eR32G32B32Sint, // st_format_r32g32b32_sint
	vk::Format::eUndefined, // st_format_r16g16b16a16_typeless
	vk::Format::eR16G16B16A16Sfloat, // st_format_r16g16b16a16_float
	vk::Format::eR16G16B16A16Unorm, // st_format_r16g16b16a16_unorm
	vk::Format::eR16G16B16A16Uint, // st_format_r16g16b16a16_uint
	vk::Format::eR16G16B16A16Snorm, // st_format_r16g16b16a16_snorm
	vk::Format::eR16G16B16A16Sint, // st_format_r16g16b16a16_sint
	vk::Format::eUndefined, // st_format_r32g32_typeless
	vk::Format::eR32G32Sfloat, // st_format_r32g32_float
	vk::Format::eR32G32Uint, // st_format_r32g32_uint
	vk::Format::eR32G32Sint, // st_format_r32g32_sint
	vk::Format::eUndefined, // st_format_r32g8x24_typeless
	vk::Format::eD32SfloatS8Uint, // st_format_d32_float_s8x24_uint
	vk::Format::eUndefined, // st_format_r32_float_x8x24_typeless
	vk::Format::eUndefined, // st_format_x32_typeless_g8x24_uint
	vk::Format::eUndefined, // st_format_r10g10b10a2_typeless
	vk::Format::eA2R10G10B10UnormPack32, // st_format_r10g10b10a2_unorm
	vk::Format::eA2R10G10B10UintPack32, // st_format_r10g10b10a2_uint
	vk::Format::eB10G11R11UfloatPack32, // st_format_r11g11b10_float
	vk::Format::eUndefined, // st_format_r8g8b8a8_typeless
	vk::Format::eR8G8B8A8Unorm, // st_format_r8g8b8a8_unorm
	vk::Format::eR8G8B8A8Srgb, // st_format_r8g8b8a8_unorm_srgb
	vk::Format::eR8G8B8A8Uint, // st_format_r8g8b8a8_uint
	vk::Format::eR8G8B8A8Snorm, // st_format_r8g8b8a8_snorm
	vk::Format::eR8G8B8A8Sint, // st_format_r8g8b8a8_sint
	vk::Format::eUndefined, // st_format_r16g16_typeless
	vk::Format::eR16G16Sfloat, // st_format_r16g16_float
	vk::Format::eR16G16Unorm, // st_format_r16g16_unorm
	vk::Format::eR16G16Uint, // st_format_r16g16_uint
	vk::Format::eR16G16Snorm, // st_format_r16g16_snorm
	vk::Format::eR16G16Sint, // st_format_r16g16_sint
	vk::Format::eUndefined, // st_format_r32_typeless
	vk::Format::eD32Sfloat, // st_format_d32_float
	vk::Format::eR32Sfloat, // st_format_r32_float
	vk::Format::eR32Uint, // st_format_r32_uint
	vk::Format::eR32Sint, // st_format_r32_sint
	vk::Format::eUndefined, // st_format_r24g8_typeless
	vk::Format::eD24UnormS8Uint, // st_format_d24_unorm_s8_uint
	vk::Format::eUndefined, // st_format_r24_unorm_x8_typeless
	vk::Format::eUndefined, // st_format_x24_typeless_g8_uint
	vk::Format::eUndefined, // st_format_r8g8_typeless
	vk::Format::eR8G8Unorm, // st_format_r8g8_unorm
	vk::Format::eR8G8Uint, // st_format_r8g8_uint
	vk::Format::eR8G8Snorm, // st_format_r8g8_snorm
	vk::Format::eR8G8Sint, // st_format_r8g8_sint
	vk::Format::eUndefined, // st_format_r16_typeless
	vk::Format::eR16Sfloat, // st_format_r16_float
	vk::Format::eD16Unorm, // st_format_d16_unorm
	vk::Format::eR16Unorm, // st_format_r16_unorm
	vk::Format::eR16Uint, // st_format_r16_uint
	vk::Format::eR16Snorm, // st_format_r16_snorm
	vk::Format::eR16Sint, // st_format_r16_sint
	vk::Format::eUndefined, // st_format_r8_typeless
	vk::Format::eR8Unorm, // st_format_r8_unorm
	vk::Format::eR8Uint, // st_format_r8_uint
	vk::Format::eR8Snorm, // st_format_r8_snorm
	vk::Format::eR8Sint, // st_format_r8_sint
	vk::Format::eUndefined, // st_format_a8_unorm
	vk::Format::eUndefined, // st_format_r1_unorm
	vk::Format::eE5B9G9R9UfloatPack32, // st_format_r9g9b9e5_sharedexp
	vk::Format::eG8B8G8R8422Unorm, // st_format_r8g8_b8g8_unorm
	vk::Format::eB8G8R8G8422Unorm, // st_format_g8r8_g8b8_unorm
	vk::Format::eUndefined, // st_format_bc1_typeless
	vk::Format::eBc1RgbaUnormBlock, // st_format_bc1_unorm
	vk::Format::eBc1RgbaSrgbBlock, // st_format_bc1_unorm_srgb
	vk::Format::eUndefined, // st_format_bc2_typeless
	vk::Format::eBc2UnormBlock, // st_format_bc2_unorm
	vk::Format::eBc2SrgbBlock, // st_format_bc2_unorm_srgb
	vk::Format::eUndefined, // st_format_bc3_typeless
	vk::Format::eBc3UnormBlock, // st_format_bc3_unorm
	vk::Format::eBc3SrgbBlock, // st_format_bc3_unorm_srgb
	vk::Format::eUndefined, // st_format_bc4_typeless
	vk::Format::eBc4UnormBlock, // st_format_bc4_unorm
	vk::Format::eBc4SnormBlock, // st_format_bc4_snorm
	vk::Format::eUndefined, // st_format_bc5_typeless
	vk::Format::eBc5UnormBlock, // st_format_bc5_unorm
	vk::Format::eBc5SnormBlock, // st_format_bc5_snorm
	vk::Format::eB5G6R5UnormPack16, // st_format_b5g6r5_unorm
	vk::Format::eB5G5R5A1UnormPack16, // st_format_b5g5r5a1_unorm
	vk::Format::eB8G8R8A8Unorm, // st_format_b8g8r8a8_unorm
	vk::Format::eB8G8R8Unorm, // st_format_b8g8r8x8_unorm
	vk::Format::eUndefined, // st_format_r10g10b10_xr_bias_a2_unorm
	vk::Format::eUndefined, // st_format_b8g8r8a8_typeless
	vk::Format::eB8G8R8A8Srgb, // st_format_b8g8r8a8_unorm_srgb
	vk::Format::eUndefined, // st_format_b8g8r8x8_typeless
	vk::Format::eB8G8R8Srgb, // st_format_b8g8r8x8_unorm_srgb
	vk::Format::eUndefined, // st_format_bc6h_typeless
	vk::Format::eBc6HUfloatBlock, // st_format_bc6h_uf16
	vk::Format::eBc6HSfloatBlock, // st_format_bc6h_sf16
	vk::Format::eUndefined, // st_format_bc7_typeless
	vk::Format::eBc7UnormBlock, // st_format_bc7_unorm
	vk::Format::eBc7SrgbBlock, // st_format_bc7_unorm_srgb
	vk::Format::eUndefined, // st_format_ayuv
	vk::Format::eUndefined, // st_format_y410
	vk::Format::eUndefined, // st_format_y416
	vk::Format::eUndefined, // st_format_nv12
	vk::Format::eUndefined, // st_format_p010
	vk::Format::eUndefined, // st_format_p016
	vk::Format::eUndefined, // st_format_420_opaque
	vk::Format::eUndefined, // st_format_yuy2
	vk::Format::eUndefined, // st_format_y210
	vk::Format::eUndefined, // st_format_y216
	vk::Format::eUndefined, // st_format_nv11
	vk::Format::eUndefined, // st_format_ai44
	vk::Format::eUndefined, // st_format_ia44
	vk::Format::eUndefined, // st_format_p8
	vk::Format::eUndefined, // st_format_a8p8
	vk::Format::eB4G4R4A4UnormPack16, // st_format_b4g4r4a4_unorm
	vk::Format::eUndefined, // st_format_p208
	vk::Format::eUndefined, // st_format_v208
	vk::Format::eUndefined, // st_format_v408
	vk::Format::eUndefined, // st_format_force_uint
};

inline vk::Format convert_format(e_st_format format)
{
	return format_mappings[format];
}

static vk::BlendFactor blend_mappings[] =
{
	vk::BlendFactor::eZero, // st_blend_zero
	vk::BlendFactor::eOne, // st_blend_one
	vk::BlendFactor::eSrcColor, // st_blend_src_color
	vk::BlendFactor::eOneMinusSrcColor, // st_blend_inv_src_color
	vk::BlendFactor::eSrcAlpha, // st_blend_src_alpha
	vk::BlendFactor::eOneMinusSrcAlpha, // st_blend_inv_src_alpha
	vk::BlendFactor::eDstAlpha, // st_blend_dst_alpha
	vk::BlendFactor::eOneMinusDstAlpha, // st_blend_inv_dst_alpha
	vk::BlendFactor::eDstColor, // st_blend_dst_color
	vk::BlendFactor::eOneMinusDstColor, // st_blend_inv_dst_color
	vk::BlendFactor::eSrcAlphaSaturate, // st_blend_src_alpha_sat
	vk::BlendFactor::eConstantColor, // st_blend_blend_factor
	vk::BlendFactor::eOneMinusConstantColor, // st_blend_inv_blend_factor
	vk::BlendFactor::eSrc1Color, // st_blend_src1_color
	vk::BlendFactor::eOneMinusSrc1Color, // st_blend_inv_src1_color
	vk::BlendFactor::eSrc1Alpha, // st_blend_src1_alpha
	vk::BlendFactor::eOneMinusSrc1Alpha, // st_blend_inv_src1_alpha
};

inline vk::BlendFactor convert_blend(e_st_blend blend)
{
	return blend_mappings[blend];
}

static vk::BlendOp blend_op_mappings[] =
{
	vk::BlendOp::eAdd, // st_blend_op_add
	vk::BlendOp::eSubtract, // st_blend_op_sub
	vk::BlendOp::eReverseSubtract, // st_blend_op_rev_sub
	vk::BlendOp::eMin, // st_blend_op_min
	vk::BlendOp::eMax, // st_blend_op_max
};

inline vk::BlendOp convert_blend_op(e_st_blend_op blend_op)
{
	return blend_op_mappings[blend_op];
}

static vk::LogicOp logic_op_mappings[] =
{
	vk::LogicOp::eClear, // st_logic_op_clear
	vk::LogicOp::eSet, // st_logic_op_set
	vk::LogicOp::eCopy, // st_logic_op_copy
	vk::LogicOp::eCopyInverted, // st_logic_op_copy_inverted
	vk::LogicOp::eNoOp, // st_logic_op_noop
	vk::LogicOp::eInvert, // st_logic_op_invert
	vk::LogicOp::eAnd, // st_logic_op_and
	vk::LogicOp::eNand, // st_logic_op_nand
	vk::LogicOp::eOr, // st_logic_op_or
	vk::LogicOp::eNor, // st_logic_op_nor
	vk::LogicOp::eXor, // st_logic_op_xor
	vk::LogicOp::eEquivalent, // st_logic_op_equiv
	vk::LogicOp::eAndReverse, // st_logic_op_and_reverse
	vk::LogicOp::eAndInverted, // st_logic_op_and_inverted
	vk::LogicOp::eOrReverse, // st_logic_op_or_reverse
	vk::LogicOp::eOrInverted, // st_logic_op_or_inverted
};

inline vk::LogicOp convert_logic_op(e_st_logic_op logic_op)
{
	return logic_op_mappings[logic_op];
}

static vk::PolygonMode fill_mode_mappings[] =
{
	vk::PolygonMode::eLine, // st_fill_mode_wireframe
	vk::PolygonMode::eFill, // st_fill_mode_solid
};

inline vk::PolygonMode convert_fill_mode(e_st_fill_mode mode)
{
	return fill_mode_mappings[mode];
}

static vk::CullModeFlagBits cull_mode_mappings[] =
{
	vk::CullModeFlagBits::eNone, // st_cull_mode_none
	vk::CullModeFlagBits::eFront, // st_cull_mode_front
	vk::CullModeFlagBits::eBack, // st_cull_mode_back
};

inline vk::CullModeFlagBits convert_cull_mode(e_st_cull_mode mode)
{
	return cull_mode_mappings[mode];
}

static uint32_t depth_write_mask_mappings[] =
{
	0, // st_depth_write_mask_zero
	1, // st_depth_write_mask_all
};

inline uint32_t convert_depth_write_mask(e_st_depth_write_mask mask)
{
	return depth_write_mask_mappings[mask];
}

static vk::CompareOp compare_op_mappings[] =
{
	vk::CompareOp::eNever, // st_compare_func_never
	vk::CompareOp::eLess, // st_compare_func_less
	vk::CompareOp::eEqual, // st_compare_func_equal
	vk::CompareOp::eLessOrEqual, // st_compare_func_less_equal
	vk::CompareOp::eGreater, // st_compare_func_greater
	vk::CompareOp::eNotEqual, // st_compare_func_not_equal
	vk::CompareOp::eGreaterOrEqual, // st_compare_func_greater_equal
	vk::CompareOp::eAlways, // st_compare_func_always
};

inline vk::CompareOp convert_compare_op(e_st_compare_func func)
{
	return compare_op_mappings[func];
}

static vk::StencilOp stencil_op_mappings[] =
{
	vk::StencilOp::eKeep, // st_stencil_op_keep
	vk::StencilOp::eZero, // st_stencil_op_zero
	vk::StencilOp::eReplace, // st_stencil_op_replace
	vk::StencilOp::eIncrementAndClamp, // st_stencil_op_incr_sat
	vk::StencilOp::eDecrementAndClamp, // st_stencil_op_decr_sat
	vk::StencilOp::eInvert, // st_stencil_op_invert
	vk::StencilOp::eIncrementAndWrap, // st_stencil_op_incr
	vk::StencilOp::eDecrementAndWrap, // st_stencil_op_decr
};

inline vk::StencilOp convert_stencil_op(e_st_stencil_op stencil_op)
{
	return stencil_op_mappings[stencil_op];
}

static vk::ImageLayout resource_state_mappings[] =
{
	vk::ImageLayout::eUndefined, // st_texture_state_common
	vk::ImageLayout::eColorAttachmentOptimal, // st_texture_state_render_target
	vk::ImageLayout::eDepthStencilAttachmentOptimal, // st_texture_state_depth_stencil_target
	vk::ImageLayout::eDepthAttachmentOptimal, // st_texture_state_depth_target
	vk::ImageLayout::eShaderReadOnlyOptimal, // st_texture_state_non_pixel_shader_read
	vk::ImageLayout::eShaderReadOnlyOptimal, // st_texture_state_pixel_shader_read
	vk::ImageLayout::eDepthReadOnlyOptimal, // st_texture_state_depth_read
	vk::ImageLayout::ePresentSrcKHR, // st_texture_state_present
	vk::ImageLayout::eTransferSrcOptimal, // st_texture_state_copy_source
	vk::ImageLayout::eTransferDstOptimal, // st_texture_state_copy_dest
	vk::ImageLayout::eGeneral, // st_texture_state_unordered_access
};

inline vk::ImageLayout convert_resource_state(e_st_texture_state state)
{
	return resource_state_mappings[state];
}

static vk::AttachmentLoadOp load_op_mappings[] =
{
	vk::AttachmentLoadOp::eLoad, // e_st_load_op::load
	vk::AttachmentLoadOp::eClear, // e_st_load_op::clear
	vk::AttachmentLoadOp::eDontCare, // e_st_load_op::dont_care
};

inline vk::AttachmentLoadOp convert_load_op(e_st_load_op op)
{
	return load_op_mappings[uint32_t(op)];
}

static vk::AttachmentStoreOp store_op_mappings[] =
{
	vk::AttachmentStoreOp::eStore, // e_st_store_op::store
	vk::AttachmentStoreOp::eDontCare, // e_st_store_op::dont_care
};

inline vk::AttachmentStoreOp convert_store_op(e_st_store_op op)
{
	return store_op_mappings[uint32_t(op)];
};

static vk::Filter filter_mappings[] =
{
	vk::Filter::eNearest,
	vk::Filter::eLinear,
};

inline vk::Filter convert_filter(e_st_filter filter)
{
	return filter_mappings[filter];
}

static vk::SamplerAddressMode address_mode_mappings[] =
{
	vk::SamplerAddressMode::eRepeat,
	vk::SamplerAddressMode::eMirrorClampToEdge,
	vk::SamplerAddressMode::eClampToEdge,
	vk::SamplerAddressMode::eClampToBorder,
};

inline vk::SamplerAddressMode convert_address_mode(e_st_address_mode mode)
{
	return address_mode_mappings[mode];
}

#endif
