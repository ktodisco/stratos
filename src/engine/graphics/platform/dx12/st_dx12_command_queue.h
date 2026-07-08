#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

class st_dx12_command_queue : public st_command_queue
{
public:

	st_dx12_command_queue(ID3D12CommandQueue* command_queue);
	~st_dx12_command_queue();

	void signal(struct st_fence* fence, uint64_t value) override;
	void wait(struct st_fence* fence, uint64_t value) override;
	void wait_for_idle(struct st_fence* fence, uint64_t value) override;
	void execute(class st_command_list* command_list) override;
	void present(struct st_swap_chain* swap_chain) override;

	ID3D12CommandQueue* get() { return _d3d_command_queue.Get(); }

private:

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _d3d_command_queue;
	HANDLE _fence_event;
};

#endif
