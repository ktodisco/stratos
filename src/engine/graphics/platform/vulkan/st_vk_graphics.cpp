/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics.h>

#include <cassert>

bool vk_validate(vk::Result result)
{
	bool ret = true;

	if (result != vk::Result::eSuccess)
	{
		assert(false);
		ret = false;
	}

	return ret;
}
