#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>
#include <graphics/st_graphics_context.h>

#include <memory>

struct st_global_resources
{
	std::unique_ptr<st_sampler> _trilinear_clamp_sampler = nullptr;
};

extern std::unique_ptr<st_global_resources> _global_resources;

void create_global_resources(st_graphics_context* context);
void destroy_global_resources();
