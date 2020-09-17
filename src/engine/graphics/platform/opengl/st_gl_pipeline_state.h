#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/st_pipeline_state_desc.h>

class st_gl_pipeline_state
{
public:
	st_gl_pipeline_state(
		const st_pipeline_state_desc& desc,
		const class st_render_pass* render_pass);
	~st_gl_pipeline_state();

	const st_pipeline_state_desc& get_state() const { return _state_desc; }

private:
	struct st_pipeline_state_desc _state_desc;
};

#endif
