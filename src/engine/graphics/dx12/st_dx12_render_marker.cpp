/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/dx12/st_dx12_render_marker.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/dx12/st_dx12_render_context.h>

#include <string>

st_dx12_render_marker::st_dx12_render_marker(const std::string& message)
{
	st_dx12_render_context::get()->begin_marker(message);
}

st_dx12_render_marker::~st_dx12_render_marker()
{
	st_dx12_render_context::get()->end_marker();
}

#endif
