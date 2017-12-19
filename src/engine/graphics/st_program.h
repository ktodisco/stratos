#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <cstdint>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/opengl/st_gl_program.h>

typedef st_gl_uniform st_platform_uniform;
typedef st_gl_shader st_platform_shader;
typedef st_gl_program st_platform_program;
#elif defined(ST_GRAPHICS_API_DX12)
#error DX12 not implemented.
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

/*
** Represents a shader uniform (constant).
** @see st_shader
*/
class st_uniform : public st_platform_uniform
{
public:
	st_uniform(int32_t location) : st_platform_uniform(location) {}
};

/*
** Represents a shader.
** @see st_program
*/
class st_shader : public st_platform_shader
{
public:
	st_shader(const char* source, uint32_t type) : st_platform_shader(source, type) {}
};

/*
** Represents a full shading pipeline.
** Vertex shader and fragment shader linked together.
** @see st_shader
*/
class st_program : public st_platform_program
{
public:
	st_program() : st_platform_program() {}
};
