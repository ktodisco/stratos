/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_command_queue.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_command_list.h>

#include <cassert>

st_dx12_command_queue::st_dx12_command_queue(ID3D12CommandQueue* command_queue)
	: _d3d_command_queue(command_queue)
{
}

st_dx12_command_queue::~st_dx12_command_queue()
{
	_d3d_command_queue = nullptr;
}

void st_dx12_command_queue::signal(st_fence* fence_, uint64_t value)
{
	st_dx12_fence* fence = static_cast<st_dx12_fence*>(fence_);

	_d3d_command_queue->Signal(fence->_fence.Get(), value);
}

void st_dx12_command_queue::wait(st_fence* fence_, uint64_t value)
{
	st_dx12_fence* fence = static_cast<st_dx12_fence*>(fence_);
	_d3d_command_queue->Wait(fence->_fence.Get(), value);
}

void st_dx12_command_queue::execute(st_command_list* command_list_)
{
	st_dx12_command_list* command_list = static_cast<st_dx12_command_list*>(command_list_);

	ID3D12CommandList* command_lists[] = { command_list->get() };
	_d3d_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);
}

void st_dx12_command_queue::present(st_swap_chain* swap_chain_, uint32_t index)
{
	st_dx12_swap_chain* swap_chain = static_cast<st_dx12_swap_chain*>(swap_chain_);

	swap_chain->_swap_chain_3->Present(1, 0);
}

#endif
