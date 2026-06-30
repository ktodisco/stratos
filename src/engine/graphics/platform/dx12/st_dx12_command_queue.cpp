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
	: _command_queue(command_queue)
{
	_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (_fence_event == nullptr)
	{
		if (GetLastError() != S_OK)
		{
			assert(false);
		}
	}
}

st_dx12_command_queue::~st_dx12_command_queue()
{
	_command_queue = nullptr;
}

void st_dx12_command_queue::signal(struct st_fence* fence_)
{
	st_dx12_fence* fence = static_cast<st_dx12_fence*>(fence_);

	const uint64_t fence_value = fence->_fence_value;
	_command_queue->Signal(fence->_fence.Get(), fence_value);
	fence->_fence_value++;
}

void st_dx12_command_queue::wait(struct st_fence* fence_)
{
	st_dx12_fence* fence = static_cast<st_dx12_fence*>(fence_);

	if (fence->_fence->GetCompletedValue() < (fence->_fence_value - 1))
	{
		fence->_fence->SetEventOnCompletion(fence->_fence_value, _fence_event);
		WaitForSingleObject(_fence_event, INFINITE);
	}
}

void st_dx12_command_queue::execute(st_command_list* command_list_)
{
	st_dx12_command_list* command_list = static_cast<st_dx12_command_list*>(command_list_);

	ID3D12CommandList* command_lists[] = { command_list->get() };
	_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);
}

void st_dx12_command_queue::present(st_swap_chain* swap_chain_)
{
	st_dx12_swap_chain* swap_chain = static_cast<st_dx12_swap_chain*>(swap_chain_);

	swap_chain->_swap_chain_3->Present(1, 0);

	// TODO: What to do about this?
	/*
	_frame_index = swap_chain->_swap_chain_3->GetCurrentBackBufferIndex();
	*/
}

#endif
