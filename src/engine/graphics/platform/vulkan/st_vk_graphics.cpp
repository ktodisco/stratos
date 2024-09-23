/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_graphics.h>

#include <cassert>

#include <debugapi.h>

vk::DispatchLoaderDynamic vk::defaultDispatchLoaderDynamic = vk::DispatchLoaderDynamic();

bool vk_validate(vk::Result result)
{
	bool ret = true;

	if (IsDebuggerPresent() && result != vk::Result::eSuccess)
	{
		__debugbreak();
		ret = false;
	}

	return ret;
}

#endif
