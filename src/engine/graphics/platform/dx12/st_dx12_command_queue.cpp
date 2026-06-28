/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_command_queue.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_command_list.h>

st_dx12_command_queue::st_dx12_command_queue(ID3D12CommandQueue* command_queue)
	: _command_queue(command_queue)
{
}

st_dx12_command_queue::~st_dx12_command_queue()
{
	_command_queue = nullptr;
}

void st_dx12_command_queue::execute(st_command_list* command_list_)
{
	st_dx12_command_list* command_list = static_cast<st_dx12_command_list*>(command_list_);

	ID3D12CommandList* command_lists[] = { command_list->get() };
	_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

	// TODO: What to do about this?
	// TODO: Better parallelization.
	/*
	const uint64_t fence = _fence_value;
	_command_queue->Signal(_fence.Get(), fence);
	_fence_value++;

	if (_fence->GetCompletedValue() < fence)
	{
		_fence->SetEventOnCompletion(fence, _fence_event);
		WaitForSingleObject(_fence_event, INFINITE);
	}
	*/
}

void st_dx12_command_queue::present(st_swap_chain* swap_chain_)
{
	st_dx12_swap_chain* swap_chain = static_cast<st_dx12_swap_chain*>(swap_chain_);

	swap_chain->_swap_chain_3->Present(1, 0);

	// TODO: What to do about this?
	// TODO: Better parallelization.
	/*
	const uint64_t fence = _fence_value;
	_command_queue->Signal(_fence.Get(), fence);
	_fence_value++;

	if (_fence->GetCompletedValue() < fence)
	{
		_fence->SetEventOnCompletion(fence, _fence_event);
		WaitForSingleObject(_fence_event, INFINITE);
	}

	_frame_index = swap_chain->_swap_chain_3->GetCurrentBackBufferIndex();
	*/
}

void st_dx12_command_queue::wait_for_idle()
{
	/*
	const uint64_t fence = _fence_value;
	_command_queue->Signal(_fence.Get(), fence);
	_fence_value++;

	if (_fence->GetCompletedValue() < fence)
	{
		_fence->SetEventOnCompletion(fence, _fence_event);
		WaitForSingleObject(_fence_event, INFINITE);
	}
	*/
}

#endif
