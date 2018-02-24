#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/st_pipeline_state_desc.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/opengl/st_gl_pipeline_state.h>

typedef st_gl_pipeline_state st_platform_pipeline_state;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/dx12/st_dx12_pipeline_state.h>

typedef st_dx12_pipeline_state st_platform_pipeline_state;
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

class st_pipeline_state : public st_platform_pipeline_state
{
public:
	st_pipeline_state(const struct st_pipeline_state_desc& desc) :
		st_platform_pipeline_state(desc) {}
};
