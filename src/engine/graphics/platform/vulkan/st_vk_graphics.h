#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_DISABLE_ENHANCED_MODE
#define VULKAN_HPP_DISABLE_IMPLICIT_RESULT_VALUE_CAST
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan.hpp>

// Ugh. Windows.
#undef min
#undef max

#define GLUEME(x, y) x##y
#define GLUE(x, y) GLUEME(x, y)
#define VK_VALIDATE(call) \
	vk::Result GLUE(vkr, __LINE__) = call; \
	vk_validate(GLUE(vkr, __LINE__))

bool vk_validate(vk::Result result);

enum e_st_primitive_topology_type
{
	st_primitive_topology_type_point = vk::PrimitiveTopology::ePointList,
	st_primitive_topology_type_line = vk::PrimitiveTopology::eLineList,
	st_primitive_topology_type_triangle = vk::PrimitiveTopology::eTriangleList,
};

enum e_st_primitive_topology
{
	st_primitive_topology_points = vk::PrimitiveTopology::ePointList,
	st_primitive_topology_lines = vk::PrimitiveTopology::eLineList,
	st_primitive_topology_triangles = vk::PrimitiveTopology::eTriangleList,
};

// HACK: In order to give a unique id to each format matching
// those of DXGI, several of which vulkan has no equivalent for.
#define VK_UNKNOWN_FORMAT 0xf000000 | __LINE__

enum e_st_format
{
	st_format_unknown = vk::Format::eUndefined,
	st_format_r32g32b32a32_typeless = VK_UNKNOWN_FORMAT,
	st_format_r32g32b32a32_float = vk::Format::eR32G32B32A32Sfloat,
	st_format_r32g32b32a32_uint = vk::Format::eR32G32B32A32Uint,
	st_format_r32g32b32a32_sint = vk::Format::eR32G32B32A32Sint,
	st_format_r32g32b32_typeless = VK_UNKNOWN_FORMAT,
	st_format_r32g32b32_float = vk::Format::eR32G32B32Sfloat,
	st_format_r32g32b32_uint = vk::Format::eR32G32B32Uint,
	st_format_r32g32b32_sint = vk::Format::eR32G32B32Sint,
	st_format_r16g16b16a16_typeless = VK_UNKNOWN_FORMAT,
	st_format_r16g16b16a16_float = vk::Format::eR16G16B16A16Sfloat,
	st_format_r16g16b16a16_unorm = vk::Format::eR16G16B16A16Unorm,
	st_format_r16g16b16a16_uint = vk::Format::eR16G16B16A16Uint,
	st_format_r16g16b16a16_snorm = vk::Format::eR16G16B16A16Snorm,
	st_format_r16g16b16a16_sint = vk::Format::eR16G16B16A16Sint,
	st_format_r32g32_typeless = VK_UNKNOWN_FORMAT,
	st_format_r32g32_float = vk::Format::eR32G32Sfloat,
	st_format_r32g32_uint = vk::Format::eR32G32Uint,
	st_format_r32g32_sint = vk::Format::eR32G32Sint,
	st_format_r32g8x24_typeless = VK_UNKNOWN_FORMAT,
	st_format_d32_float_s8x24_uint = vk::Format::eD32SfloatS8Uint,
	st_format_r32_float_x8x24_typeless = VK_UNKNOWN_FORMAT,
	st_format_x32_typeless_g8x24_uint = VK_UNKNOWN_FORMAT,
	st_format_r10g10b10a2_typeless = VK_UNKNOWN_FORMAT,
	st_format_r10g10b10a2_unorm = vk::Format::eA2R10G10B10UnormPack32,
	st_format_r10g10b10a2_uint = vk::Format::eA2R10G10B10UintPack32,
	st_format_r11g11b10_float = vk::Format::eB10G11R11UfloatPack32,
	st_format_r8g8b8a8_typeless = VK_UNKNOWN_FORMAT,
	st_format_r8g8b8a8_unorm = vk::Format::eR8G8B8A8Unorm,
	st_format_r8g8b8a8_unorm_srgb = vk::Format::eR8G8B8A8Srgb,
	st_format_r8g8b8a8_uint = vk::Format::eR8G8B8A8Uint,
	st_format_r8g8b8a8_snorm = vk::Format::eR8G8B8A8Snorm,
	st_format_r8g8b8a8_sint = vk::Format::eR8G8B8A8Sint,
	st_format_r16g16_typeless = VK_UNKNOWN_FORMAT,
	st_format_r16g16_float = vk::Format::eR16G16Sfloat,
	st_format_r16g16_unorm = vk::Format::eR16G16Unorm,
	st_format_r16g16_uint = vk::Format::eR16G16Uint,
	st_format_r16g16_snorm = vk::Format::eR16G16Snorm,
	st_format_r16g16_sint = vk::Format::eR16G16Sint,
	st_format_r32_typeless = VK_UNKNOWN_FORMAT,
	st_format_d32_float = vk::Format::eD32Sfloat,
	st_format_r32_float = vk::Format::eR32Sfloat,
	st_format_r32_uint = vk::Format::eR32Uint,
	st_format_r32_sint = vk::Format::eR32Sint,
	st_format_r24g8_typeless = VK_UNKNOWN_FORMAT,
	st_format_d24_unorm_s8_uint = vk::Format::eD24UnormS8Uint,
	st_format_r24_unorm_x8_typeless = VK_UNKNOWN_FORMAT,
	st_format_x24_typeless_g8_uint = VK_UNKNOWN_FORMAT,
	st_format_r8g8_typeless = VK_UNKNOWN_FORMAT,
	st_format_r8g8_unorm = vk::Format::eR8G8Unorm,
	st_format_r8g8_uint = vk::Format::eR8G8Uint,
	st_format_r8g8_snorm = vk::Format::eR8G8Snorm,
	st_format_r8g8_sint = vk::Format::eR8G8Sint,
	st_format_r16_typeless = VK_UNKNOWN_FORMAT,
	st_format_r16_float = vk::Format::eR16Sfloat,
	st_format_d16_unorm = vk::Format::eD16Unorm,
	st_format_r16_unorm = vk::Format::eR16Unorm,
	st_format_r16_uint = vk::Format::eR16Uint,
	st_format_r16_snorm = vk::Format::eR16Snorm,
	st_format_r16_sint = vk::Format::eR16Sint,
	st_format_r8_typeless = VK_UNKNOWN_FORMAT,
	st_format_r8_unorm = vk::Format::eR8Unorm,
	st_format_r8_uint = vk::Format::eR8Uint,
	st_format_r8_snorm = vk::Format::eR8Snorm,
	st_format_r8_sint = vk::Format::eR8Sint,
	st_format_a8_unorm = VK_UNKNOWN_FORMAT,
	st_format_r1_unorm = VK_UNKNOWN_FORMAT,
	st_format_r9g9b9e5_sharedexp = vk::Format::eE5B9G9R9UfloatPack32,
	st_format_r8g8_b8g8_unorm = vk::Format::eG8B8G8R8422Unorm,
	st_format_g8r8_g8b8_unorm = vk::Format::eB8G8R8G8422Unorm,
	st_format_bc1_typeless = VK_UNKNOWN_FORMAT,
	st_format_bc1_unorm = vk::Format::eBc1RgbaUnormBlock,
	st_format_bc1_unorm_srgb = vk::Format::eBc1RgbaSrgbBlock,
	st_format_bc2_typeless = VK_UNKNOWN_FORMAT,
	st_format_bc2_unorm = vk::Format::eBc2UnormBlock,
	st_format_bc2_unorm_srgb = vk::Format::eBc2SrgbBlock,
	st_format_bc3_typeless = VK_UNKNOWN_FORMAT,
	st_format_bc3_unorm = vk::Format::eBc3UnormBlock,
	st_format_bc3_unorm_srgb = vk::Format::eBc3SrgbBlock,
	st_format_bc4_typeless = VK_UNKNOWN_FORMAT,
	st_format_bc4_unorm = vk::Format::eBc4UnormBlock,
	st_format_bc4_snorm = vk::Format::eBc4SnormBlock,
	st_format_bc5_typeless = VK_UNKNOWN_FORMAT,
	st_format_bc5_unorm = vk::Format::eBc5UnormBlock,
	st_format_bc5_snorm = vk::Format::eBc5SnormBlock,
	st_format_b5g6r5_unorm = vk::Format::eB5G6R5UnormPack16,
	st_format_b5g5r5a1_unorm = vk::Format::eB5G5R5A1UnormPack16,
	st_format_b8g8r8a8_unorm = vk::Format::eB8G8R8A8Unorm,
	st_format_b8g8r8x8_unorm = vk::Format::eB8G8R8Unorm,
	st_format_r10g10b10_xr_bias_a2_unorm = VK_UNKNOWN_FORMAT,
	st_format_b8g8r8a8_typeless = VK_UNKNOWN_FORMAT,
	st_format_b8g8r8a8_unorm_srgb = vk::Format::eB8G8R8A8Srgb,
	st_format_b8g8r8x8_typeless = VK_UNKNOWN_FORMAT,
	st_format_b8g8r8x8_unorm_srgb = vk::Format::eB8G8R8Srgb,
	st_format_bc6h_typeless = VK_UNKNOWN_FORMAT,
	st_format_bc6h_uf16 = vk::Format::eBc6HUfloatBlock,
	st_format_bc6h_sf16 = vk::Format::eBc6HSfloatBlock,
	st_format_bc7_typeless = VK_UNKNOWN_FORMAT,
	st_format_bc7_unorm = vk::Format::eBc7UnormBlock,
	st_format_bc7_unorm_srgb = vk::Format::eBc7SrgbBlock,
	st_format_ayuv = VK_UNKNOWN_FORMAT,
	st_format_y410 = VK_UNKNOWN_FORMAT,
	st_format_y416 = VK_UNKNOWN_FORMAT,
	st_format_nv12 = VK_UNKNOWN_FORMAT,
	st_format_p010 = VK_UNKNOWN_FORMAT,
	st_format_p016 = VK_UNKNOWN_FORMAT,
	st_format_420_opaque = VK_UNKNOWN_FORMAT,
	st_format_yuy2 = VK_UNKNOWN_FORMAT,
	st_format_y210 = VK_UNKNOWN_FORMAT,
	st_format_y216 = VK_UNKNOWN_FORMAT,
	st_format_nv11 = VK_UNKNOWN_FORMAT,
	st_format_ai44 = VK_UNKNOWN_FORMAT,
	st_format_ia44 = VK_UNKNOWN_FORMAT,
	st_format_p8 = VK_UNKNOWN_FORMAT,
	st_format_a8p8 = VK_UNKNOWN_FORMAT,
	st_format_b4g4r4a4_unorm = vk::Format::eB4G4R4A4UnormPack16,
	st_format_p208 = VK_UNKNOWN_FORMAT,
	st_format_v208 = VK_UNKNOWN_FORMAT,
	st_format_v408 = VK_UNKNOWN_FORMAT,
	st_format_force_uint = VK_UNKNOWN_FORMAT
};

enum e_st_blend
{
	st_blend_zero = vk::BlendFactor::eZero,
	st_blend_one = vk::BlendFactor::eOne,
	st_blend_src_color = vk::BlendFactor::eSrcColor,
	st_blend_inv_src_color = vk::BlendFactor::eOneMinusSrcColor,
	st_blend_src_alpha = vk::BlendFactor::eSrcAlpha,
	st_blend_inv_src_alpha = vk::BlendFactor::eOneMinusSrcAlpha,
	st_blend_dst_alpha = vk::BlendFactor::eDstAlpha,
	st_blend_inv_dst_alpha = vk::BlendFactor::eOneMinusDstAlpha,
	st_blend_dst_color = vk::BlendFactor::eDstColor,
	st_blend_inv_dst_color = vk::BlendFactor::eOneMinusDstColor,
	st_blend_src_alpha_sat = vk::BlendFactor::eSrcAlphaSaturate,
	st_blend_blend_factor = vk::BlendFactor::eConstantColor,
	st_blend_inv_blend_factor = vk::BlendFactor::eOneMinusConstantColor,
	st_blend_src1_color = vk::BlendFactor::eSrc1Color,
	st_blend_inv_src1_color = vk::BlendFactor::eOneMinusSrc1Color,
	st_blend_src1_alpha = vk::BlendFactor::eSrc1Alpha,
	st_blend_inv_src1_alpha = vk::BlendFactor::eOneMinusSrc1Alpha,
};

enum e_st_blend_op
{
	st_blend_op_add = vk::BlendOp::eAdd,
	st_blend_op_sub = vk::BlendOp::eSubtract,
	st_blend_op_rev_sub = vk::BlendOp::eReverseSubtract,
	st_blend_op_min = vk::BlendOp::eMin,
	st_blend_op_max = vk::BlendOp::eMax,
};

enum e_st_logic_op
{
	st_logic_op_clear = vk::LogicOp::eClear,
	st_logic_op_set = vk::LogicOp::eSet,
	st_logic_op_copy = vk::LogicOp::eCopy,
	st_logic_op_copy_inverted = vk::LogicOp::eCopyInverted,
	st_logic_op_noop = vk::LogicOp::eNoOp,
	st_logic_op_invert = vk::LogicOp::eInvert,
	st_logic_op_and = vk::LogicOp::eAnd,
	st_logic_op_nand = vk::LogicOp::eNand,
	st_logic_op_or = vk::LogicOp::eOr,
	st_logic_op_nor = vk::LogicOp::eNor,
	st_logic_op_xor = vk::LogicOp::eXor,
	st_logic_op_equiv = vk::LogicOp::eEquivalent,
	st_logic_op_and_reverse = vk::LogicOp::eAndReverse,
	st_logic_op_and_inverted = vk::LogicOp::eAndInverted,
	st_logic_op_or_reverse = vk::LogicOp::eOrReverse,
	st_logic_op_or_inverted = vk::LogicOp::eOrInverted,
};

enum e_st_fill_mode
{
	st_fill_mode_wireframe = vk::PolygonMode::eLine,
	st_fill_mode_solid = vk::PolygonMode::eFill,
};

enum e_st_cull_mode
{
	st_cull_mode_none = vk::CullModeFlagBits::eNone,
	st_cull_mode_front = vk::CullModeFlagBits::eFront,
	st_cull_mode_back = vk::CullModeFlagBits::eBack,
};

enum e_st_depth_write_mask
{
	st_depth_write_mask_zero = 0,
	st_depth_write_mask_all = 1,
};

enum e_st_compare_func
{
	st_compare_func_never = vk::CompareOp::eNever,
	st_compare_func_less = vk::CompareOp::eLess,
	st_compare_func_equal = vk::CompareOp::eEqual,
	st_compare_func_less_equal = vk::CompareOp::eLessOrEqual,
	st_compare_func_greater = vk::CompareOp::eGreater,
	st_compare_func_not_equal = vk::CompareOp::eNotEqual,
	st_compare_func_greater_equal = vk::CompareOp::eGreaterOrEqual,
	st_compare_func_always = vk::CompareOp::eAlways,
};

enum e_st_stencil_op
{
	st_stencil_op_keep = vk::StencilOp::eKeep,
	st_stencil_op_zero = vk::StencilOp::eZero,
	st_stencil_op_replace = vk::StencilOp::eReplace,
	st_stencil_op_incr_sat = vk::StencilOp::eIncrementAndClamp,
	st_stencil_op_decr_sat = vk::StencilOp::eDecrementAndClamp,
	st_stencil_op_invert = vk::StencilOp::eInvert,
	st_stencil_op_incr = vk::StencilOp::eIncrementAndWrap,
	st_stencil_op_decr = vk::StencilOp::eDecrementAndWrap,
};

enum e_st_texture_state
{
	st_texture_state_common = vk::ImageLayout::eUndefined,
	st_texture_state_render_target = vk::ImageLayout::eColorAttachmentOptimal,
	st_texture_state_depth_stencil_target = vk::ImageLayout::eDepthStencilAttachmentOptimal,
	st_texture_state_depth_target = vk::ImageLayout::eDepthAttachmentOptimal,
	st_texture_state_non_pixel_shader_read = vk::ImageLayout::eShaderReadOnlyOptimal,
	st_texture_state_pixel_shader_read = vk::ImageLayout::eShaderReadOnlyOptimal,
	st_texture_state_depth_read = vk::ImageLayout::eDepthReadOnlyOptimal,
	st_texture_state_present = vk::ImageLayout::ePresentSrcKHR,
	st_texture_state_copy_source = vk::ImageLayout::eTransferSrcOptimal,
	st_texture_state_copy_dest = vk::ImageLayout::eTransferDstOptimal,
};
