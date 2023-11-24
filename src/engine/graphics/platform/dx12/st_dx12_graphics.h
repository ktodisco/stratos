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

#include <graphics/platform/dx12/d3dx12.h>

#include <memory>
#include <vector>

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

enum e_st_format
{
	st_format_unknown = DXGI_FORMAT_UNKNOWN,
	st_format_r32g32b32a32_typeless = DXGI_FORMAT_R32G32B32A32_TYPELESS,
	st_format_r32g32b32a32_float = DXGI_FORMAT_R32G32B32A32_FLOAT,
	st_format_r32g32b32a32_uint = DXGI_FORMAT_R32G32B32A32_UINT,
	st_format_r32g32b32a32_sint = DXGI_FORMAT_R32G32B32A32_SINT,
	st_format_r32g32b32_typeless = DXGI_FORMAT_R32G32B32_TYPELESS,
	st_format_r32g32b32_float = DXGI_FORMAT_R32G32B32_FLOAT,
	st_format_r32g32b32_uint = DXGI_FORMAT_R32G32B32_UINT,
	st_format_r32g32b32_sint = DXGI_FORMAT_R32G32B32_SINT,
	st_format_r16g16b16a16_typeless = DXGI_FORMAT_R16G16B16A16_TYPELESS,
	st_format_r16g16b16a16_float = DXGI_FORMAT_R16G16B16A16_FLOAT,
	st_format_r16g16b16a16_unorm = DXGI_FORMAT_R16G16B16A16_UNORM,
	st_format_r16g16b16a16_uint = DXGI_FORMAT_R16G16B16A16_UINT,
	st_format_r16g16b16a16_snorm = DXGI_FORMAT_R16G16B16A16_SNORM,
	st_format_r16g16b16a16_sint = DXGI_FORMAT_R16G16B16A16_SINT,
	st_format_r32g32_typeless = DXGI_FORMAT_R32G32_TYPELESS,
	st_format_r32g32_float = DXGI_FORMAT_R32G32_FLOAT,
	st_format_r32g32_uint = DXGI_FORMAT_R32G32_UINT,
	st_format_r32g32_sint = DXGI_FORMAT_R32G32_SINT,
	st_format_r32g8x24_typeless = DXGI_FORMAT_R32G8X24_TYPELESS,
	st_format_d32_float_s8x24_uint = DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
	st_format_r32_float_x8x24_typeless = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
	st_format_x32_typeless_g8x24_uint = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
	st_format_r10g10b10a2_typeless = DXGI_FORMAT_R10G10B10A2_TYPELESS,
	st_format_r10g10b10a2_unorm = DXGI_FORMAT_R10G10B10A2_UNORM,
	st_format_r10g10b10a2_uint = DXGI_FORMAT_R10G10B10A2_UINT,
	st_format_r11g11b10_float = DXGI_FORMAT_R11G11B10_FLOAT,
	st_format_r8g8b8a8_typeless = DXGI_FORMAT_R8G8B8A8_TYPELESS,
	st_format_r8g8b8a8_unorm = DXGI_FORMAT_R8G8B8A8_UNORM,
	st_format_r8g8b8a8_unorm_srgb = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	st_format_r8g8b8a8_uint = DXGI_FORMAT_R8G8B8A8_UINT,
	st_format_r8g8b8a8_snorm = DXGI_FORMAT_R8G8B8A8_SNORM,
	st_format_r8g8b8a8_sint = DXGI_FORMAT_R8G8B8A8_SINT,
	st_format_r16g16_typeless = DXGI_FORMAT_R16G16_TYPELESS,
	st_format_r16g16_float = DXGI_FORMAT_R16G16_FLOAT,
	st_format_r16g16_unorm = DXGI_FORMAT_R16G16_UNORM,
	st_format_r16g16_uint = DXGI_FORMAT_R16G16_UINT,
	st_format_r16g16_snorm = DXGI_FORMAT_R16G16_SNORM,
	st_format_r16g16_sint = DXGI_FORMAT_R16G16_SINT,
	st_format_r32_typeless = DXGI_FORMAT_R32_TYPELESS,
	st_format_d32_float = DXGI_FORMAT_D32_FLOAT,
	st_format_r32_float = DXGI_FORMAT_R32_FLOAT,
	st_format_r32_uint = DXGI_FORMAT_R32_UINT,
	st_format_r32_sint = DXGI_FORMAT_R32_SINT,
	st_format_r24g8_typeless = DXGI_FORMAT_R24G8_TYPELESS,
	st_format_d24_unorm_s8_uint = DXGI_FORMAT_D24_UNORM_S8_UINT,
	st_format_r24_unorm_x8_typeless = DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
	st_format_x24_typeless_g8_uint = DXGI_FORMAT_X24_TYPELESS_G8_UINT,
	st_format_r8g8_typeless = DXGI_FORMAT_R8G8_TYPELESS,
	st_format_r8g8_unorm = DXGI_FORMAT_R8G8_UNORM,
	st_format_r8g8_uint = DXGI_FORMAT_R8G8_UINT,
	st_format_r8g8_snorm = DXGI_FORMAT_R8G8_SNORM,
	st_format_r8g8_sint = DXGI_FORMAT_R8G8_SINT,
	st_format_r16_typeless = DXGI_FORMAT_R16_TYPELESS,
	st_format_r16_float = DXGI_FORMAT_R16_FLOAT,
	st_format_d16_unorm = DXGI_FORMAT_D16_UNORM,
	st_format_r16_unorm = DXGI_FORMAT_R16_UNORM,
	st_format_r16_uint = DXGI_FORMAT_R16_UINT,
	st_format_r16_snorm = DXGI_FORMAT_R16_SNORM,
	st_format_r16_sint = DXGI_FORMAT_R16_SINT,
	st_format_r8_typeless = DXGI_FORMAT_R8_TYPELESS,
	st_format_r8_unorm = DXGI_FORMAT_R8_UNORM,
	st_format_r8_uint = DXGI_FORMAT_R8_UINT,
	st_format_r8_snorm = DXGI_FORMAT_R8_SNORM,
	st_format_r8_sint = DXGI_FORMAT_R8_SINT,
	st_format_a8_unorm = DXGI_FORMAT_A8_UNORM,
	st_format_r1_unorm = DXGI_FORMAT_R1_UNORM,
	st_format_r9g9b9e5_sharedexp = DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
	st_format_r8g8_b8g8_unorm = DXGI_FORMAT_R8G8_B8G8_UNORM,
	st_format_g8r8_g8b8_unorm = DXGI_FORMAT_G8R8_G8B8_UNORM,
	st_format_bc1_typeless = DXGI_FORMAT_BC1_TYPELESS,
	st_format_bc1_unorm = DXGI_FORMAT_BC1_UNORM,
	st_format_bc1_unorm_srgb = DXGI_FORMAT_BC1_UNORM_SRGB,
	st_format_bc2_typeless = DXGI_FORMAT_BC2_TYPELESS,
	st_format_bc2_unorm = DXGI_FORMAT_BC2_UNORM,
	st_format_bc2_unorm_srgb = DXGI_FORMAT_BC2_UNORM_SRGB,
	st_format_bc3_typeless = DXGI_FORMAT_BC3_TYPELESS,
	st_format_bc3_unorm = DXGI_FORMAT_BC3_UNORM,
	st_format_bc3_unorm_srgb = DXGI_FORMAT_BC3_UNORM_SRGB,
	st_format_bc4_typeless = DXGI_FORMAT_BC4_TYPELESS,
	st_format_bc4_unorm = DXGI_FORMAT_BC4_UNORM,
	st_format_bc4_snorm = DXGI_FORMAT_BC4_SNORM,
	st_format_bc5_typeless = DXGI_FORMAT_BC5_TYPELESS,
	st_format_bc5_unorm = DXGI_FORMAT_BC5_UNORM,
	st_format_bc5_snorm = DXGI_FORMAT_BC5_SNORM,
	st_format_b5g6r5_unorm = DXGI_FORMAT_B5G6R5_UNORM,
	st_format_b5g5r5a1_unorm = DXGI_FORMAT_B5G5R5A1_UNORM,
	st_format_b8g8r8a8_unorm = DXGI_FORMAT_B8G8R8A8_UNORM,
	st_format_b8g8r8x8_unorm = DXGI_FORMAT_B8G8R8X8_UNORM,
	st_format_r10g10b10_xr_bias_a2_unorm = DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
	st_format_b8g8r8a8_typeless = DXGI_FORMAT_B8G8R8A8_TYPELESS,
	st_format_b8g8r8a8_unorm_srgb = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
	st_format_b8g8r8x8_typeless = DXGI_FORMAT_B8G8R8X8_TYPELESS,
	st_format_b8g8r8x8_unorm_srgb = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
	st_format_bc6h_typeless = DXGI_FORMAT_BC6H_TYPELESS,
	st_format_bc6h_uf16 = DXGI_FORMAT_BC6H_UF16,
	st_format_bc6h_sf16 = DXGI_FORMAT_BC6H_SF16,
	st_format_bc7_typeless = DXGI_FORMAT_BC7_TYPELESS,
	st_format_bc7_unorm = DXGI_FORMAT_BC7_UNORM,
	st_format_bc7_unorm_srgb = DXGI_FORMAT_BC7_UNORM_SRGB,
	st_format_ayuv = DXGI_FORMAT_AYUV,
	st_format_y410 = DXGI_FORMAT_Y410,
	st_format_y416 = DXGI_FORMAT_Y416,
	st_format_nv12 = DXGI_FORMAT_NV12,
	st_format_p010 = DXGI_FORMAT_P010,
	st_format_p016 = DXGI_FORMAT_P016,
	st_format_420_opaque = DXGI_FORMAT_420_OPAQUE,
	st_format_yuy2 = DXGI_FORMAT_YUY2,
	st_format_y210 = DXGI_FORMAT_Y210,
	st_format_y216 = DXGI_FORMAT_Y216,
	st_format_nv11 = DXGI_FORMAT_NV11,
	st_format_ai44 = DXGI_FORMAT_AI44,
	st_format_ia44 = DXGI_FORMAT_IA44,
	st_format_p8 = DXGI_FORMAT_P8,
	st_format_a8p8 = DXGI_FORMAT_A8P8,
	st_format_b4g4r4a4_unorm = DXGI_FORMAT_B4G4R4A4_UNORM,
	st_format_p208 = DXGI_FORMAT_P208,
	st_format_v208 = DXGI_FORMAT_V208,
	st_format_v408 = DXGI_FORMAT_V408,
	st_format_force_uint = DXGI_FORMAT_FORCE_UINT
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

enum e_st_texture_state
{
	st_texture_state_common = D3D12_RESOURCE_STATE_COMMON,
	st_texture_state_render_target = D3D12_RESOURCE_STATE_RENDER_TARGET,
	st_texture_state_depth_stencil_target = D3D12_RESOURCE_STATE_RENDER_TARGET,
	st_texture_state_depth_target = D3D12_RESOURCE_STATE_DEPTH_WRITE,
	st_texture_state_non_pixel_shader_read = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
	st_texture_state_pixel_shader_read = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	st_texture_state_depth_read = D3D12_RESOURCE_STATE_DEPTH_READ,
	st_texture_state_present = D3D12_RESOURCE_STATE_PRESENT,
	st_texture_state_copy_source = D3D12_RESOURCE_STATE_COPY_SOURCE,
	st_texture_state_copy_dest = D3D12_RESOURCE_STATE_COPY_DEST,
};

typedef uint32_t st_dx12_descriptor;

struct st_dx12_buffer : public st_buffer
{
	uint32_t _count;
	e_st_buffer_usage_flags _usage;
	size_t _element_size;
	Microsoft::WRL::ComPtr<ID3D12Resource> _buffer;
	uint8_t* _buffer_head = nullptr;
};

struct st_dx12_buffer_view : public st_buffer_view
{
	union
	{
		D3D12_VERTEX_BUFFER_VIEW vertex;
		D3D12_INDEX_BUFFER_VIEW index;
	};
};

struct st_dx12_constant_buffer : public st_constant_buffer
{
	size_t _size;
	Microsoft::WRL::ComPtr<ID3D12Resource> _constant_buffer;
	uint8_t* _constant_buffer_head = nullptr;
};

struct st_dx12_pipeline : public st_pipeline
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipeline;
};

struct st_dx12_render_pass : public st_render_pass
{
	std::unique_ptr<class st_dx12_framebuffer> _framebuffer;
	st_viewport _viewport;
};

struct st_dx12_resource_table : public st_resource_table
{
	std::vector<st_dx12_descriptor> _cbvs;
	std::vector<st_dx12_descriptor> _srvs;
	std::vector<st_dx12_descriptor> _samplers;
	std::vector<st_dx12_descriptor> _buffers;
};

struct st_dx12_shader : public st_shader
{
	Microsoft::WRL::ComPtr<ID3DBlob> _vs;
	Microsoft::WRL::ComPtr<ID3DBlob> _ps;
	Microsoft::WRL::ComPtr<ID3DBlob> _ds;
	Microsoft::WRL::ComPtr<ID3DBlob> _hs;
	Microsoft::WRL::ComPtr<ID3DBlob> _gs;

	uint8_t _type = 0;
};

struct st_dx12_texture : public st_texture
{
	Microsoft::WRL::ComPtr<ID3D12Resource> _handle;

	uint32_t _width;
	uint32_t _height;
	uint32_t _levels = 1;
	e_st_format _format;
	e_st_texture_usage_flags _usage = static_cast<e_st_texture_usage_flags>(0);
	e_st_texture_state _state = st_texture_state_common;
};

struct st_dx12_render_texture : public st_dx12_texture
{
	st_dx12_descriptor _rtv;
};

struct st_dx12_vertex_format : public st_vertex_format
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> _elements;
	D3D12_INPUT_LAYOUT_DESC _input_layout = {};
};
