#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <core/st_flags.h>

#include <math/st_vec4f.h>

#include <memory>
#include <string>

#define ST_GRAPHICS_API_OPENGL
#define ST_GRAPHICS_API_DX12
#define ST_GRAPHICS_API_VULKAN

// TODO: Find an ideal way to represent these.
#define k_max_frames 2
#define k_max_loaded_resources 4096
#define k_max_shader_resources 4096
#define k_max_samplers 128

enum e_st_primitive_topology_type
{
	st_primitive_topology_type_point,
	st_primitive_topology_type_line,
	st_primitive_topology_type_triangle,
};

enum e_st_primitive_topology
{
	st_primitive_topology_points,
	st_primitive_topology_lines,
	st_primitive_topology_triangles,
};

enum e_st_format
{
	st_format_unknown,
	st_format_r32g32b32a32_typeless,
	st_format_r32g32b32a32_float,
	st_format_r32g32b32a32_uint,
	st_format_r32g32b32a32_sint,
	st_format_r32g32b32_typeless,
	st_format_r32g32b32_float,
	st_format_r32g32b32_uint,
	st_format_r32g32b32_sint,
	st_format_r16g16b16a16_typeless,
	st_format_r16g16b16a16_float,
	st_format_r16g16b16a16_unorm,
	st_format_r16g16b16a16_uint,
	st_format_r16g16b16a16_snorm,
	st_format_r16g16b16a16_sint,
	st_format_r32g32_typeless,
	st_format_r32g32_float,
	st_format_r32g32_uint,
	st_format_r32g32_sint,
	st_format_r32g8x24_typeless,
	st_format_d32_float_s8x24_uint,
	st_format_r32_float_x8x24_typeless,
	st_format_x32_typeless_g8x24_uint,
	st_format_r10g10b10a2_typeless,
	st_format_r10g10b10a2_unorm,
	st_format_r10g10b10a2_uint,
	st_format_r11g11b10_float,
	st_format_r8g8b8a8_typeless,
	st_format_r8g8b8a8_unorm,
	st_format_r8g8b8a8_unorm_srgb,
	st_format_r8g8b8a8_uint,
	st_format_r8g8b8a8_snorm,
	st_format_r8g8b8a8_sint,
	st_format_r16g16_typeless,
	st_format_r16g16_float,
	st_format_r16g16_unorm,
	st_format_r16g16_uint,
	st_format_r16g16_snorm,
	st_format_r16g16_sint,
	st_format_r32_typeless,
	st_format_d32_float,
	st_format_r32_float,
	st_format_r32_uint,
	st_format_r32_sint,
	st_format_r24g8_typeless,
	st_format_d24_unorm_s8_uint,
	st_format_r24_unorm_x8_typeless,
	st_format_x24_typeless_g8_uint,
	st_format_r8g8_typeless,
	st_format_r8g8_unorm,
	st_format_r8g8_uint,
	st_format_r8g8_snorm,
	st_format_r8g8_sint,
	st_format_r16_typeless,
	st_format_r16_float,
	st_format_d16_unorm,
	st_format_r16_unorm,
	st_format_r16_uint,
	st_format_r16_snorm,
	st_format_r16_sint,
	st_format_r8_typeless,
	st_format_r8_unorm,
	st_format_r8_uint,
	st_format_r8_snorm,
	st_format_r8_sint,
	st_format_a8_unorm,
	st_format_r1_unorm,
	st_format_r9g9b9e5_sharedexp,
	st_format_r8g8_b8g8_unorm,
	st_format_g8r8_g8b8_unorm,
	st_format_bc1_typeless,
	st_format_bc1_unorm,
	st_format_bc1_unorm_srgb,
	st_format_bc2_typeless,
	st_format_bc2_unorm,
	st_format_bc2_unorm_srgb,
	st_format_bc3_typeless,
	st_format_bc3_unorm,
	st_format_bc3_unorm_srgb,
	st_format_bc4_typeless,
	st_format_bc4_unorm,
	st_format_bc4_snorm,
	st_format_bc5_typeless,
	st_format_bc5_unorm,
	st_format_bc5_snorm,
	st_format_b5g6r5_unorm,
	st_format_b5g5r5a1_unorm,
	st_format_b8g8r8a8_unorm,
	st_format_b8g8r8x8_unorm,
	st_format_r10g10b10_xr_bias_a2_unorm,
	st_format_b8g8r8a8_typeless,
	st_format_b8g8r8a8_unorm_srgb,
	st_format_b8g8r8x8_typeless,
	st_format_b8g8r8x8_unorm_srgb,
	st_format_bc6h_typeless,
	st_format_bc6h_uf16,
	st_format_bc6h_sf16,
	st_format_bc7_typeless,
	st_format_bc7_unorm,
	st_format_bc7_unorm_srgb,
	st_format_ayuv,
	st_format_y410,
	st_format_y416,
	st_format_nv12,
	st_format_p010,
	st_format_p016,
	st_format_420_opaque,
	st_format_yuy2,
	st_format_y210,
	st_format_y216,
	st_format_nv11,
	st_format_ai44,
	st_format_ia44,
	st_format_p8,
	st_format_a8p8,
	st_format_b4g4r4a4_unorm,
	st_format_p208,
	st_format_v208,
	st_format_v408,
	st_format_force_uint
};

enum e_st_blend
{
	st_blend_zero,
	st_blend_one,
	st_blend_src_color,
	st_blend_inv_src_color,
	st_blend_src_alpha,
	st_blend_inv_src_alpha,
	st_blend_dst_alpha,
	st_blend_inv_dst_alpha,
	st_blend_dst_color,
	st_blend_inv_dst_color,
	st_blend_src_alpha_sat,
	st_blend_blend_factor,
	st_blend_inv_blend_factor,
	st_blend_src1_color,
	st_blend_inv_src1_color,
	st_blend_src1_alpha,
	st_blend_inv_src1_alpha,
};

enum e_st_blend_op
{
	st_blend_op_add,
	st_blend_op_sub,
	st_blend_op_rev_sub,
	st_blend_op_min,
	st_blend_op_max,
};

enum e_st_logic_op
{
	st_logic_op_clear,
	st_logic_op_set,
	st_logic_op_copy,
	st_logic_op_copy_inverted,
	st_logic_op_noop,
	st_logic_op_invert,
	st_logic_op_and,
	st_logic_op_nand,
	st_logic_op_or,
	st_logic_op_nor,
	st_logic_op_xor,
	st_logic_op_equiv,
	st_logic_op_and_reverse,
	st_logic_op_and_inverted,
	st_logic_op_or_reverse,
	st_logic_op_or_inverted,
};

enum e_st_fill_mode
{
	st_fill_mode_wireframe,
	st_fill_mode_solid,
};

enum e_st_cull_mode
{
	st_cull_mode_none,
	st_cull_mode_front,
	st_cull_mode_back,
};

enum e_st_depth_write_mask
{
	st_depth_write_mask_zero,
	st_depth_write_mask_all,
};

enum e_st_compare_func
{
	st_compare_func_never,
	st_compare_func_less,
	st_compare_func_equal,
	st_compare_func_less_equal,
	st_compare_func_greater,
	st_compare_func_not_equal,
	st_compare_func_greater_equal,
	st_compare_func_always,
};

enum e_st_stencil_op
{
	st_stencil_op_keep,
	st_stencil_op_zero,
	st_stencil_op_replace,
	st_stencil_op_incr_sat,
	st_stencil_op_decr_sat,
	st_stencil_op_invert,
	st_stencil_op_incr,
	st_stencil_op_decr,
};

enum e_st_texture_state
{
	st_texture_state_common,
	st_texture_state_render_target,
	st_texture_state_depth_stencil_target,
	st_texture_state_depth_target,
	st_texture_state_non_pixel_shader_read,
	st_texture_state_pixel_shader_read,
	st_texture_state_depth_read,
	st_texture_state_present,
	st_texture_state_copy_source,
	st_texture_state_copy_dest,
};

enum e_st_filter
{
	st_filter_nearest,
	st_filter_linear
};

enum e_st_address_mode
{
	st_address_mode_wrap,
	st_address_mode_mirror,
	st_address_mode_clamp,
	st_address_mode_border
};

enum e_st_descriptor_slot
{
	st_descriptor_slot_textures,
	st_descriptor_slot_samplers,
	st_descriptor_slot_constants,
	st_descriptor_slot_buffers,

	st_descriptor_slot_count
};

enum class e_st_buffer_usage : uint32_t
{
	index = 0x0001,
	indirect = 0x0002,
	storage = 0x0004,
	storage_texel = 0x0008,
	transfer_dest = 0x0010,
	transfer_source = 0x0020,
	uniform = 0x0040,
	uniform_texel = 0x0080,
	vertex = 0x0100,
};
using e_st_buffer_usage_flags = st_flags<e_st_buffer_usage, uint32_t>;
ST_ENUM_FLAG_OPS(e_st_buffer_usage, e_st_buffer_usage_flags);

enum class e_st_texture_usage : uint32_t
{
	copy_source = 0x0001,
	copy_dest = 0x0002,
	sampled = 0x0004,
	storage = 0x0008,
	color_target = 0x0010,
	depth_target = 0x0020,
	transient_target = 0x0040,
	input_target = 0x0080,
};
using e_st_texture_usage_flags = st_flags<e_st_texture_usage, uint32_t>;
ST_ENUM_FLAG_OPS(e_st_texture_usage, e_st_texture_usage_flags);

enum e_st_clear_flags
{
	st_clear_flag_color = 1,
	st_clear_flag_depth = 2,
	st_clear_flag_stencil = 4
};

// TODO: DirectX packing rules do not allow for vec2s or vec3s.
// It's probably best just to deprecate them.
enum e_st_shader_constant_type
{
	st_shader_constant_type_float,
	st_shader_constant_type_vec2,
	st_shader_constant_type_vec3,
	st_shader_constant_type_vec4,
	st_shader_constant_type_mat4,
	st_shader_constant_type_block,
};

enum class e_st_load_op : uint8_t
{
	load,
	clear,
	dont_care,
};

enum class e_st_store_op : uint8_t
{
	store,
	dont_care,
};

enum class e_st_graphics_api : uint8_t
{
	dx12,
	opengl,
	vulkan,
};

struct st_viewport
{
	float _x;
	float _y;
	float _width;
	float _height;
	float _min_depth;
	float _max_depth;
};

// The virtual destructor is leveraged to release API resources when they
// go out of scope. Implementation is API-specific.
struct st_resource
{
	virtual ~st_resource() {}
};

struct st_depth_stencil_clear_value
{
	float _depth;
	uint32_t _stencil;
};

struct st_clear_value
{
	st_clear_value() = delete;
	st_clear_value(const st_vec4f& v) : _color(v) {}
	st_clear_value(const st_depth_stencil_clear_value& v) : _depth_stencil(v) {}

	union
	{
		st_vec4f _color;
		st_depth_stencil_clear_value _depth_stencil;
	};
};

struct st_sampler_desc
{
	e_st_filter _min_filter = st_filter_nearest;
	e_st_filter _mag_filter = st_filter_nearest;
	e_st_filter _mip_filter = st_filter_nearest;

	e_st_address_mode _address_u = st_address_mode_clamp;
	e_st_address_mode _address_v = st_address_mode_clamp;
	e_st_address_mode _address_w = st_address_mode_clamp;

	uint32_t _anisotropy = 0;

	e_st_compare_func _compare_func = st_compare_func_never;

	st_vec4f _border = st_vec4f::zero_vector();

	float _mip_bias = 0.0f;
	float _min_mip = 0;
	float _max_mip = FLT_MAX;
};

struct st_texture_desc
{
	uint32_t _width = 1;
	uint32_t _height = 1;
	uint32_t _depth = 1;
	uint32_t _levels = 1;

	e_st_format _format = st_format_unknown;
	e_st_texture_usage_flags _usage = e_st_texture_usage::sampled;
	e_st_texture_state _initial_state = st_texture_state_common;

	st_clear_value _clear = st_vec4f::zero_vector();

	void* _data = nullptr;
};

struct st_buffer : public st_resource {};
struct st_buffer_view : public st_resource {};
struct st_pipeline : public st_resource {};
struct st_render_pass : public st_resource {};
struct st_resource_table : public st_resource {};
struct st_sampler : public st_resource {};
struct st_shader : public st_resource {};
struct st_texture : public st_resource {};
struct st_texture_view : public st_resource {};
struct st_vertex_format : public st_resource
{
	uint32_t _vertex_size;
};

struct st_range
{
	size_t begin;
	size_t end;
};

struct st_target_desc
{
	class st_render_texture* _target = nullptr;
	e_st_load_op _load_op = e_st_load_op::clear;
	e_st_store_op _store_op = e_st_store_op::store;
};

size_t st_graphics_get_shader_constant_size(e_st_shader_constant_type constant_type);

#include <cstdint>

template<typename T>
static T align_value(T value, uint32_t alignment)
{
	return (value + ((T)alignment - 1)) & ~((T)alignment - 1);
}

inline bool is_compressed(e_st_format fmt)
{
	switch (fmt)
	{
	case st_format_bc1_typeless:
	case st_format_bc1_unorm:
	case st_format_bc1_unorm_srgb:
	case st_format_bc2_typeless:
	case st_format_bc2_unorm:
	case st_format_bc2_unorm_srgb:
	case st_format_bc3_typeless:
	case st_format_bc3_unorm:
	case st_format_bc3_unorm_srgb:
	case st_format_bc4_typeless:
	case st_format_bc4_unorm:
	case st_format_bc4_snorm:
	case st_format_bc5_typeless:
	case st_format_bc5_unorm:
	case st_format_bc5_snorm:
	case st_format_bc6h_typeless:
	case st_format_bc6h_uf16:
	case st_format_bc6h_sf16:
	case st_format_bc7_typeless:
	case st_format_bc7_unorm:
	case st_format_bc7_unorm_srgb:
		return true;

	default:
		return false;
	}
}

inline size_t bits_per_pixel(e_st_format fmt)
{
	switch (fmt)
	{
	case st_format_r32g32b32a32_typeless:
	case st_format_r32g32b32a32_float:
	case st_format_r32g32b32a32_uint:
	case st_format_r32g32b32a32_sint:
		return 128;

	case st_format_r32g32b32_typeless:
	case st_format_r32g32b32_float:
	case st_format_r32g32b32_uint:
	case st_format_r32g32b32_sint:
		return 96;

	case st_format_r16g16b16a16_typeless:
	case st_format_r16g16b16a16_float:
	case st_format_r16g16b16a16_unorm:
	case st_format_r16g16b16a16_uint:
	case st_format_r16g16b16a16_snorm:
	case st_format_r16g16b16a16_sint:
	case st_format_r32g32_typeless:
	case st_format_r32g32_float:
	case st_format_r32g32_uint:
	case st_format_r32g32_sint:
	case st_format_r32g8x24_typeless:
	case st_format_d32_float_s8x24_uint:
	case st_format_r32_float_x8x24_typeless:
	case st_format_x32_typeless_g8x24_uint:
	case st_format_y416:
	case st_format_y210:
	case st_format_y216:
		return 64;

	case st_format_r10g10b10a2_typeless:
	case st_format_r10g10b10a2_unorm:
	case st_format_r10g10b10a2_uint:
	case st_format_r11g11b10_float:
	case st_format_r8g8b8a8_typeless:
	case st_format_r8g8b8a8_unorm:
	case st_format_r8g8b8a8_unorm_srgb:
	case st_format_r8g8b8a8_uint:
	case st_format_r8g8b8a8_snorm:
	case st_format_r8g8b8a8_sint:
	case st_format_r16g16_typeless:
	case st_format_r16g16_float:
	case st_format_r16g16_unorm:
	case st_format_r16g16_uint:
	case st_format_r16g16_snorm:
	case st_format_r16g16_sint:
	case st_format_r32_typeless:
	case st_format_d32_float:
	case st_format_r32_float:
	case st_format_r32_uint:
	case st_format_r32_sint:
	case st_format_r24g8_typeless:
	case st_format_d24_unorm_s8_uint:
	case st_format_r24_unorm_x8_typeless:
	case st_format_x24_typeless_g8_uint:
	case st_format_r9g9b9e5_sharedexp:
	case st_format_r8g8_b8g8_unorm:
	case st_format_g8r8_g8b8_unorm:
	case st_format_b8g8r8a8_unorm:
	case st_format_b8g8r8x8_unorm:
	case st_format_r10g10b10_xr_bias_a2_unorm:
	case st_format_b8g8r8a8_typeless:
	case st_format_b8g8r8a8_unorm_srgb:
	case st_format_b8g8r8x8_typeless:
	case st_format_b8g8r8x8_unorm_srgb:
	case st_format_ayuv:
	case st_format_y410:
	case st_format_yuy2:
		return 32;

	case st_format_p010:
	case st_format_p016:
		return 24;

	case st_format_r8g8_typeless:
	case st_format_r8g8_unorm:
	case st_format_r8g8_uint:
	case st_format_r8g8_snorm:
	case st_format_r8g8_sint:
	case st_format_r16_typeless:
	case st_format_r16_float:
	case st_format_d16_unorm:
	case st_format_r16_unorm:
	case st_format_r16_uint:
	case st_format_r16_snorm:
	case st_format_r16_sint:
	case st_format_b5g6r5_unorm:
	case st_format_b5g5r5a1_unorm:
	case st_format_a8p8:
	case st_format_b4g4r4a4_unorm:
		return 16;

	case st_format_nv12:
	case st_format_420_opaque:
	case st_format_nv11:
		return 12;

	case st_format_r8_typeless:
	case st_format_r8_unorm:
	case st_format_r8_uint:
	case st_format_r8_snorm:
	case st_format_r8_sint:
	case st_format_a8_unorm:
	case st_format_ai44:
	case st_format_ia44:
	case st_format_p8:
		return 8;

	case st_format_r1_unorm:
		return 1;

	case st_format_bc1_typeless:
	case st_format_bc1_unorm:
	case st_format_bc1_unorm_srgb:
	case st_format_bc4_typeless:
	case st_format_bc4_unorm:
	case st_format_bc4_snorm:
		return 4;

	case st_format_bc2_typeless:
	case st_format_bc2_unorm:
	case st_format_bc2_unorm_srgb:
	case st_format_bc3_typeless:
	case st_format_bc3_unorm:
	case st_format_bc3_unorm_srgb:
	case st_format_bc5_typeless:
	case st_format_bc5_unorm:
	case st_format_bc5_snorm:
	case st_format_bc6h_typeless:
	case st_format_bc6h_uf16:
	case st_format_bc6h_sf16:
	case st_format_bc7_typeless:
	case st_format_bc7_unorm:
	case st_format_bc7_unorm_srgb:
		return 8;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)

	case st_format_v408:
		return 24;

	case st_format_p208:
	case st_format_v208:
		return 16;

#endif // (_WIN32_WINNT >= _WIN32_WINNT_WIN10)

	case st_format_unknown:
	case st_format_force_uint:
	default:
		return 0;
	}
}

inline size_t bytes_per_pixel(e_st_format format)
{
	return (bits_per_pixel(format) / 8);
}

inline bool get_surface_info(
	size_t width,
	size_t height,
	e_st_format fmt,
	size_t* out_num_bytes,
	size_t* out_row_bytes,
	size_t* out_num_rows)
{
	uint64_t num_bytes = 0;
	uint64_t row_bytes = 0;
	uint64_t num_rows = 0;

	bool bc = false;
	bool packed = false;
	bool planar = false;
	size_t bpe = 0;
	switch (fmt)
	{
	case st_format_bc1_typeless:
	case st_format_bc1_unorm:
	case st_format_bc1_unorm_srgb:
	case st_format_bc4_typeless:
	case st_format_bc4_unorm:
	case st_format_bc4_snorm:
		bc = true;
		bpe = 8;
		break;

	case st_format_bc2_typeless:
	case st_format_bc2_unorm:
	case st_format_bc2_unorm_srgb:
	case st_format_bc3_typeless:
	case st_format_bc3_unorm:
	case st_format_bc3_unorm_srgb:
	case st_format_bc5_typeless:
	case st_format_bc5_unorm:
	case st_format_bc5_snorm:
	case st_format_bc6h_typeless:
	case st_format_bc6h_uf16:
	case st_format_bc6h_sf16:
	case st_format_bc7_typeless:
	case st_format_bc7_unorm:
	case st_format_bc7_unorm_srgb:
		bc = true;
		bpe = 16;
		break;

	case st_format_r8g8_b8g8_unorm:
	case st_format_g8r8_g8b8_unorm:
	case st_format_yuy2:
		packed = true;
		bpe = 4;
		break;

	case st_format_y210:
	case st_format_y216:
		packed = true;
		bpe = 8;
		break;

	case st_format_nv12:
	case st_format_420_opaque:
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	case st_format_p208:
#endif
		planar = true;
		bpe = 2;
		break;

	case st_format_p010:
	case st_format_p016:
		planar = true;
		bpe = 4;
		break;

	default:
		break;
	}

	if (bc)
	{
		uint64_t numBlocksWide = 0;
		if (width > 0)
		{
			numBlocksWide = std::max<uint64_t>(1u, (uint64_t(width) + 3u) / 4u);
		}
		uint64_t numBlocksHigh = 0;
		if (height > 0)
		{
			numBlocksHigh = std::max<uint64_t>(1u, (uint64_t(height) + 3u) / 4u);
		}
		row_bytes = numBlocksWide * bpe;
		num_rows = numBlocksHigh;
		num_bytes = row_bytes * numBlocksHigh;
	}
	else if (packed)
	{
		row_bytes = ((uint64_t(width) + 1u) >> 1) * bpe;
		num_rows = uint64_t(height);
		num_bytes = row_bytes * height;
	}
	else if (fmt == st_format_nv11)
	{
		row_bytes = ((uint64_t(width) + 3u) >> 2) * 4u;
		num_rows = uint64_t(height) * 2u; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
		num_bytes = row_bytes * num_rows;
	}
	else if (planar)
	{
		row_bytes = ((uint64_t(width) + 1u) >> 1) * bpe;
		num_bytes = (row_bytes * uint64_t(height)) + ((row_bytes * uint64_t(height) + 1u) >> 1);
		num_rows = height + ((uint64_t(height) + 1u) >> 1);
	}
	else
	{
		size_t bpp = bits_per_pixel(fmt);
		if (!bpp)
			return false;

		row_bytes = (uint64_t(width) * bpp + 7u) / 8u; // round up to nearest byte
		num_rows = uint64_t(height);
		num_bytes = row_bytes * height;
	}

#if defined(_M_IX86) || defined(_M_ARM) || defined(_M_HYBRID_X86_ARM64)
	static_assert(sizeof(size_t) == 4, "Not a 32-bit platform!");
	if (num_bytes > UINT32_MAX || row_bytes > UINT32_MAX || num_rows > UINT32_MAX)
		return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
#else
	static_assert(sizeof(size_t) == 8, "Not a 64-bit platform!");
#endif

	if (out_num_bytes)
	{
		*out_num_bytes = static_cast<size_t>(num_bytes);
	}
	if (out_row_bytes)
	{
		*out_row_bytes = static_cast<size_t>(row_bytes);
	}
	if (out_num_rows)
	{
		*out_num_rows = static_cast<size_t>(num_rows);
	}

	return true;
}
