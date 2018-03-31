#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/wglew.h>

#include <Windows.h>

#define ST_GL_CALL(call) \
	call; \
	GLenum er = glGetError(); \
	if (er != GL_NO_ERROR) \
	{ \
		int* brk = 0; \
		*brk = 0; \
	}

void gl_message_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

enum e_st_primitive_topology_type
{
	st_primitive_topology_type_point = GL_POINTS,
	st_primitive_topology_type_line = GL_LINES,
	st_primitive_topology_type_triangle = GL_TRIANGLES,
};

enum e_st_primitive_topology
{
	st_primitive_topology_points = GL_POINTS,
	st_primitive_topology_lines = GL_LINES,
	st_primitive_topology_triangles = GL_TRIANGLES,
};

enum e_st_texture_format
{
	st_texture_format_unknown = GLU_UNKNOWN,
	st_texture_format_r8_uint = GL_R8,
	st_texture_format_r8_unorm = GL_R8,
	st_texture_format_r8g8b8a8_uint = GL_RGBA8,
	st_texture_format_r8g8b8a8_unorm = GL_RGBA8,
	st_texture_format_d24_unorm_s8_uint = GL_DEPTH24_STENCIL8,
};

enum e_st_blend
{
	st_blend_zero				= GL_ZERO,
	st_blend_one				= GL_ONE,
	st_blend_src_color			= GL_SRC_COLOR,
	st_blend_inv_src_color		= GL_ONE_MINUS_SRC_COLOR,
	st_blend_src_alpha			= GL_SRC_ALPHA,
	st_blend_inv_src_alpha		= GL_ONE_MINUS_SRC_ALPHA,
	st_blend_dst_alpha			= GL_DST_ALPHA,
	st_blend_inv_dst_alpha		= GL_ONE_MINUS_DST_ALPHA,
	st_blend_dst_color			= GL_DST_COLOR,
	st_blend_inv_dst_color		= GL_ONE_MINUS_DST_COLOR,
	st_blend_src_alpha_sat		= GL_SRC_ALPHA_SATURATE,
	st_blend_blend_factor		= GL_CONSTANT_COLOR,
	st_blend_inv_blend_factor	= GL_ONE_MINUS_CONSTANT_COLOR,
	st_blend_src1_color			= GL_SRC1_COLOR,
	st_blend_inv_src1_color		= GL_ONE_MINUS_SRC1_COLOR,
	st_blend_src1_alpha			= GL_SRC1_ALPHA,
	st_blend_inv_src1_alpha		= GL_ONE_MINUS_SRC1_ALPHA,
};

enum e_st_blend_op
{
	st_blend_op_add				= GL_FUNC_ADD,
	st_blend_op_sub				= GL_FUNC_SUBTRACT,
	st_blend_op_rev_sub			= GL_FUNC_REVERSE_SUBTRACT,
	st_blend_op_min				= GL_MIN,
	st_blend_op_max				= GL_MAX,
};

enum e_st_logic_op
{
	st_logic_op_clear			= GL_CLEAR,
	st_logic_op_set				= GL_SET,
	st_logic_op_copy			= GL_COPY,
	st_logic_op_copy_inverted	= GL_COPY_INVERTED,
	st_logic_op_noop			= GL_NOOP,
	st_logic_op_invert			= GL_INVERT,
	st_logic_op_and				= GL_AND,
	st_logic_op_nand			= GL_NAND,
	st_logic_op_or				= GL_OR,
	st_logic_op_nor				= GL_NOR,
	st_logic_op_xor				= GL_XOR,
	st_logic_op_equiv			= GL_EQUIV,
	st_logic_op_and_reverse		= GL_AND_REVERSE,
	st_logic_op_and_inverted	= GL_AND_INVERTED,
	st_logic_op_or_reverse		= GL_OR_REVERSE,
	st_logic_op_or_inverted		= GL_OR_INVERTED,
};

enum e_st_fill_mode
{
	st_fill_mode_wireframe	= GL_LINE,
	st_fill_mode_solid		= GL_FILL,
};

enum e_st_cull_mode
{
	st_cull_mode_none		= GL_NONE,
	st_cull_mode_front		= GL_FRONT,
	st_cull_mode_back		= GL_BACK,
};

enum e_st_depth_write_mask
{
	st_depth_write_mask_zero		= GL_ZERO,
	st_depth_write_mask_all			= GL_ONE,
};

enum e_st_compare_func
{
	st_compare_func_never			= GL_NEVER,
	st_compare_func_less			= GL_LESS,
	st_compare_func_equal			= GL_EQUAL,
	st_compare_func_less_equal		= GL_LEQUAL,
	st_compare_func_greater			= GL_GREATER,
	st_compare_func_not_equal		= GL_NOTEQUAL,
	st_compare_func_greater_equal	= GL_GEQUAL,
	st_compare_func_always			= GL_ALWAYS,
};

enum e_st_stencil_op
{
	st_stencil_op_keep			= GL_KEEP,
	st_stencil_op_zero			= GL_ZERO,
	st_stencil_op_replace		= GL_REPLACE,
	st_stencil_op_incr_sat		= GL_INCR,
	st_stencil_op_decr_sat		= GL_DECR,
	st_stencil_op_invert		= GL_INVERT,
	st_stencil_op_incr			= GL_INCR_WRAP,
	st_stencil_op_decr			= GL_DECR_WRAP,
};
