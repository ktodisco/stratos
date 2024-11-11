/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_global_resources.h>

std::unique_ptr<st_global_resources> _global_resources = nullptr;

void create_global_resources(st_graphics_context* context)
{
	_global_resources = std::make_unique<st_global_resources>();

	{
		st_sampler_desc desc;
		desc._min_filter = st_filter_linear;
		desc._mag_filter = st_filter_linear;
		desc._mip_filter = st_filter_linear;
		desc._address_u = st_address_mode_wrap;
		desc._address_v = st_address_mode_wrap;
		desc._address_w = st_address_mode_wrap;

		_global_resources->_trilinear_clamp_sampler = context->create_sampler(desc);
	}
}

void destroy_global_resources()
{
	_global_resources = nullptr;
}