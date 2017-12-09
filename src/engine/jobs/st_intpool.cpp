/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_intpool.h"

#include <atomic>
#include <cstdint>

struct st_intpool_nodecount_t
{
	uint32_t _index;
	uint32_t _count;
};

union st_intpool_pointer_t
{
	std::atomic<uint64_t> _atomic;

	uint64_t _entire;
	st_intpool_nodecount_t _part;

	st_intpool_pointer_t() {}
	st_intpool_pointer_t(const st_intpool_pointer_t& other) : _entire(other._entire) {}
};

struct st_intpool_node_t
{
	st_intpool_pointer_t _next;
};

struct st_intpool_impl_t
{
	int _index_count;

	st_intpool_pointer_t _free_list;

	st_intpool_node_t* _nodes;
};

static const uint32_t k_st_intpool_invalid_index = 0xffffffff;

st_intpool::st_intpool(int index_count)
{
	auto impl = new st_intpool_impl_t;

	impl->_index_count = index_count;
	impl->_free_list._entire = 0;
	impl->_nodes = new st_intpool_node_t[index_count];

	for (int i = 0; i < index_count - 1; ++i)
	{
		impl->_nodes[i]._next._part._index = i + 1;
		impl->_nodes[i]._next._part._count = 0;
	}
	impl->_nodes[index_count - 1]._next._part._index = k_st_intpool_invalid_index;
	impl->_nodes[index_count - 1]._next._part._count = 0;

	_impl = impl;
}

st_intpool::~st_intpool()
{
	st_intpool_impl_t* impl = static_cast<st_intpool_impl_t*>(_impl);
	delete[] impl->_nodes;
	delete impl;
}

int st_intpool::alloc()
{
	int index;
	st_intpool_impl_t* impl = static_cast<st_intpool_impl_t*>(_impl);

	for (;;)
	{
		st_intpool_pointer_t free_list = impl->_free_list;

		if (free_list._part._index != k_st_intpool_invalid_index)
		{
			index = free_list._part._index;
			st_intpool_pointer_t next = impl->_nodes[index]._next;

			st_intpool_pointer_t link;
			link._part._index = next._part._index;
			link._part._count = free_list._part._count + 1;
			if (impl->_free_list._atomic.compare_exchange_strong(free_list._entire, link._entire))
			{
				break;
			}
		}
	}

	return index;
}

void st_intpool::free(int index)
{
	st_intpool_impl_t* impl = static_cast<st_intpool_impl_t*>(_impl);

	st_intpool_node_t* node = impl->_nodes + index;
	for (;;)
	{
		st_intpool_pointer_t free_list = impl->_free_list;
		node->_next._part._index = free_list._part._index;

		st_intpool_pointer_t link;
		link._part._index = index;
		link._part._count = free_list._part._count + 1;
		if (impl->_free_list._atomic.compare_exchange_strong(free_list._entire, link._entire))
		{
			break;
		}
	}
}

int st_intpool::get_index_count() const
{
	st_intpool_impl_t* impl = static_cast<st_intpool_impl_t*>(_impl);
	return impl->_index_count;
}
