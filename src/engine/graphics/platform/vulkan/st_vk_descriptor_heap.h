#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <cstdint>
#include <list>
#include <memory>

typedef uint32_t st_vk_descriptor;

struct st_vk_cpu_descriptor_handle
{
	//D3D12_CPU_DESCRIPTOR_HANDLE _handle;
	uint32_t _offset;
};

struct st_vk_gpu_descriptor_handle
{
	//D3D12_GPU_DESCRIPTOR_HANDLE _handle;
	uint32_t _offset;
};

struct st_descriptor_free_block
{
	uint32_t _index = 0;
	uint32_t _size = 0;
};

/*
** Management class for a DirectX 12 descriptor heap.
**
** Keeps track of total allocations vs. heap size, and the first
** free slot available.  When things are removed from the heap, a
** slot is freed.
*/
class st_vk_descriptor_heap
{
public:
	st_vk_descriptor_heap(
		//ID3D12Device* device,
		uint32_t size) {}
		//D3D12_DESCRIPTOR_HEAP_TYPE type,
		//D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	~st_vk_descriptor_heap() {}

	st_vk_cpu_descriptor_handle allocate_handle() { return {}; }
	void deallocate_handle(uint32_t offset) {}

	//ID3D12DescriptorHeap* get() const;
	//D3D12_CPU_DESCRIPTOR_HANDLE get_handle_cpu(st_dx12_descriptor offset) const;
	//D3D12_GPU_DESCRIPTOR_HANDLE get_handle_gpu(st_dx12_descriptor offset) const;
	uint32_t get_descriptor_size() const;

private:
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _heap;
	uint32_t _descriptor_size;

	std::list<std::unique_ptr<st_descriptor_free_block>> _free_blocks;
};

#endif
