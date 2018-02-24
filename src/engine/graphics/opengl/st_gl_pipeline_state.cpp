/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_pipeline_state.h>

#if defined(ST_GRAPHICS_API_OPENGL)

st_gl_pipeline_state::st_gl_pipeline_state(const st_pipeline_state_desc& desc)
{
	_state_desc = desc;
}

st_gl_pipeline_state::~st_gl_pipeline_state()
{
}

#endif
