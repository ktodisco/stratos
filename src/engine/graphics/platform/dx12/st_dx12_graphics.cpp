/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>

st_dx12_buffer_view::~st_dx12_buffer_view()
{
	_heap->deallocate_handle(_handle);
}

st_dx12_sampler::~st_dx12_sampler()
{
	_heap->deallocate_handle(_handle);
}

st_dx12_texture_view::~st_dx12_texture_view()
{
	_heap->deallocate_handle(_handle);
}