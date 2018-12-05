/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <cassert>

st_dx12_descriptor_heap::st_dx12_descriptor_heap(
	ID3D12Device* device,
	uint32_t size,
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbv_srv_heap_desc{};
	cbv_srv_heap_desc.NumDescriptors = size;
	cbv_srv_heap_desc.Type = type;
	cbv_srv_heap_desc.Flags = flags;
	HRESULT result = device->CreateDescriptorHeap(
		&cbv_srv_heap_desc,
		__uuidof(ID3D12DescriptorHeap),
		(void**)&_heap);

	_descriptor_size = device->GetDescriptorHandleIncrementSize(type);

	if (result != S_OK)
	{
		assert(false);
	}

	// Set up the initial free block.
	_free_blocks.push_front(std::make_unique<st_descriptor_free_block>());
	_free_blocks.front()->_index = 0;
	_free_blocks.front()->_size = size;
}

st_dx12_descriptor_heap::~st_dx12_descriptor_heap()
{
}

st_dx12_cpu_descriptor_handle st_dx12_descriptor_heap::allocate_handle()
{
	if (_free_blocks.size() == 0)
	{
		assert(false);
	}

	// Find the first free block.
	st_descriptor_free_block* block = _free_blocks.front().get();
	uint32_t index = block->_index;
	block->_index++;
	block->_size--;

	if (block->_size == 0)
	{
		_free_blocks.pop_front();
	}

	st_dx12_cpu_descriptor_handle handle
	{
		get_handle_cpu(index),
		index
	};

	return handle;
}

void st_dx12_descriptor_heap::deallocate_handle(uint32_t offset)
{
	bool success = false;

	for (auto itr = _free_blocks.begin(); itr != _free_blocks.end(); ++itr)
	{
		st_descriptor_free_block* block = (*itr).get();

		// Handle the case where the freed handle is at the end of an existing free block.
		if (offset == (block->_index + block->_size))
		{
			block->_size++;

			// Handle the case where this block now connects to the next block.
			auto next_block = std::next(itr, 1);
			if (next_block != _free_blocks.end())
			{
				if (block->_index + block->_size == (*next_block)->_index)
				{
					// Prefer to keep around the next block, as it's guaranteed to be older in memory and
					// will help with fragmentation.
					(*next_block)->_index = block->_index;
					(*next_block)->_size += block->_size;
					_free_blocks.pop_front();
				}
			}

			success = true;
			break;
		}

		// Handle the case where the freed handle is at the beginning of an existing free block.
		if (offset == (block->_index - 1))
		{
			block->_size++;
			block->_index = offset;
			success = true;
			break;
		}
	}

	if (!success)
	{
		// Set up the initial free block.
		_free_blocks.push_front(std::make_unique<st_descriptor_free_block>());
		_free_blocks.front()->_index = offset;
		_free_blocks.front()->_size = 1;
	}
}

ID3D12DescriptorHeap* st_dx12_descriptor_heap::get() const
{
	return _heap.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE st_dx12_descriptor_heap::get_handle_cpu(st_dx12_descriptor offset) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _heap->GetCPUDescriptorHandleForHeapStart();;
	handle.ptr += _descriptor_size * offset;

	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE st_dx12_descriptor_heap::get_handle_gpu(st_dx12_descriptor offset) const
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = _heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += _descriptor_size * offset;

	return handle;
}

uint32_t st_dx12_descriptor_heap::get_descriptor_size() const
{
	return _descriptor_size;
}

#endif
