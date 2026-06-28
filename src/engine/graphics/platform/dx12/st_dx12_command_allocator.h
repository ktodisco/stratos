#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

class st_dx12_command_allocator : public st_command_allocator
{
public:

	st_dx12_command_allocator(ID3D12CommandAllocator* command_allocator);
	~st_dx12_command_allocator();

	void reset() override;

	ID3D12CommandAllocator* get() { return _command_allocator.Get(); }

private:

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _command_allocator;
};

#endif