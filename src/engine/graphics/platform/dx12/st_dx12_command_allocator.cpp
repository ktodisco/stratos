/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_command_allocator.h>

#if defined(ST_GRAPHICS_API_DX12)

st_dx12_command_allocator::st_dx12_command_allocator(ID3D12CommandAllocator* command_allocator)
	: _command_allocator(command_allocator)
{
}

st_dx12_command_allocator::~st_dx12_command_allocator()
{
	_command_allocator = nullptr;
}

void st_dx12_command_allocator::reset()
{
	_command_allocator->Reset();
}

#endif
