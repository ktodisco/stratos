#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <string>

class st_dx12_render_marker
{
public:
	st_dx12_render_marker(const std::string& message);
	~st_dx12_render_marker();
};

#endif
