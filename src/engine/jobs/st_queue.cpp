/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_queue.h"

#include <atomic>
#include <cstdint>

struct st_queue_nodecount_t
{
	uint32_t _index;
	uint32_t _count;
};

union st_queue_pointer_t
{
	std::atomic<uint64_t> _atomic;

	uint64_t _entire;
	st_queue_nodecount_t _part;

	st_queue_pointer_t() {}
	st_queue_pointer_t(const st_queue_pointer_t& other) : _entire(other._entire) {}
	st_queue_pointer_t& operator=(const st_queue_pointer_t& other) { _entire = other._entire; return *this; }
};

struct st_queue_node_t
{
	void* _data;
	st_queue_pointer_t _next;
};

struct st_queue_impl_t
{
	st_queue_pointer_t _head;
	st_queue_pointer_t _tail;
	st_queue_pointer_t _free_list;

	std::atomic_int _count;

	st_queue_node_t* _nodes;
};

static const uint32_t k_st_queue_invalid_index = 0xffffffff;

static uint32_t _alloc_node_index(st_queue_impl_t* impl);
static void _free_node_index(st_queue_impl_t* impl, uint32_t index);
static st_queue_node_t* _init_node(st_queue_impl_t* impl, uint32_t node_index);

st_queue::st_queue(int node_count)
{
	auto impl = new st_queue_impl_t;

	impl->_count = 0;
	impl->_free_list._entire = 0;
	impl->_nodes = new st_queue_node_t[node_count];

	/* Link nodes together. */
	for (int i = 0; i < node_count - 1; ++i)
	{
		impl->_nodes[i]._next._part._index = i + 1;
		impl->_nodes[i]._next._part._count = 0;
	}
	impl->_nodes[node_count - 1]._next._part._index = k_st_queue_invalid_index;
	impl->_nodes[node_count - 1]._next._part._count = 0;

	/* Populate the queue with a dummy node. */
	uint32_t dummy_index = _alloc_node_index(impl);
	_init_node(impl, dummy_index);

	impl->_head._part._index = dummy_index;
	impl->_head._part._count = 0;
	impl->_tail._part._index = dummy_index;
	impl->_tail._part._count = 0;

	_impl = impl;
}

st_queue::~st_queue()
{
	st_queue_impl_t* impl = static_cast<st_queue_impl_t*>(_impl);
	delete[] impl->_nodes;
	delete impl;
}

void st_queue::push(void* data)
{
	st_queue_impl_t* impl = static_cast<st_queue_impl_t*>(_impl);

	/* Allocate a new node for this data. */
	uint32_t node_index = _alloc_node_index(impl);
	st_queue_node_t* node = _init_node(impl, node_index);
	node->_data = data;

	st_queue_pointer_t tail;

	/* Try until the push succeeds. */
	for (;;)
	{
		tail = impl->_tail;
		st_queue_pointer_t next = impl->_nodes[tail._part._index]._next;

		/* Is our view of the queue still consistent? If not, try again. */
		if (tail._entire == impl->_tail._entire)
		{
			/* Is tail pointing to last node? */
			if (next._part._index == k_st_queue_invalid_index)
			{
				/* Attempt to push new node onto tail. Leave the loop on success. */
				st_queue_pointer_t link;
				link._part._index = node_index;
				link._part._count = next._part._count + 1;
				if (impl->_nodes[tail._part._index]._next._atomic.compare_exchange_strong(next._entire, link._entire))
				{
					break;
				}
			}

			/* Tail has fallen behind the actual end of the queue. Fix that. */
			else
			{
				st_queue_pointer_t link;
				link._part._index = next._part._index;
				link._part._count = tail._part._count + 1;
				impl->_tail._atomic.compare_exchange_strong(tail._entire, link._entire);
			}
		}
	}

	/* Try to advance the tail pointer. We'll handle the fail case on future calls. */
	{
		st_queue_pointer_t link;
		link._part._index = node_index;
		link._part._count = tail._part._count + 1;
		impl->_tail._atomic.compare_exchange_strong(tail._entire, link._entire);
		impl->_count++;
	}
}

bool st_queue::pop(void** data)
{
	st_queue_impl_t* impl = static_cast<st_queue_impl_t*>(_impl);
	st_queue_pointer_t head;

	for (;;)
	{
		head = impl->_head;
		st_queue_pointer_t tail = impl->_tail;
		st_queue_pointer_t next = impl->_nodes[head._part._index]._next;

		/* Is our view of the queue still consistent? If not, try again. */
		if (head._entire == impl->_head._entire)
		{
			if (head._part._index == tail._part._index)
			{
				/* If queue is empty, fail the pop. */
				if (next._part._index == k_st_queue_invalid_index)
				{
					return false;
				}

				/* Tail has fallen behind the actual end of the queue. Fix that. */
				st_queue_pointer_t link;
				link._part._index = next._part._index;
				link._part._count = tail._part._count + 1;
				impl->_tail._atomic.compare_exchange_strong(tail._entire, link._entire);
			}
			else
			{
				/* Grab the data. */
				*data = impl->_nodes[next._part._index]._data;

				/* Attempt to pop the node. Leave the loop on success. */
				st_queue_pointer_t link;
				link._part._index = next._part._index;
				link._part._count = head._part._count + 1;
				if (impl->_head._atomic.compare_exchange_strong(head._entire, link._entire))
				{
					break;
				}
			}
		}
	}

	impl->_count--;
	_free_node_index(impl, head._part._index);
	return true;
}

int st_queue::get_count() const
{
	st_queue_impl_t* impl = static_cast<st_queue_impl_t*>(_impl);
	return impl->_count;
}

static uint32_t _alloc_node_index(st_queue_impl_t* impl)
{
	uint32_t index;

	for (;;)
	{
		st_queue_pointer_t free_list = impl->_free_list;

		if (free_list._part._index != k_st_queue_invalid_index)
		{
			index = free_list._part._index;
			st_queue_pointer_t next = impl->_nodes[index]._next;

			st_queue_pointer_t link;
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

static void _free_node_index(st_queue_impl_t* impl, uint32_t index)
{
	st_queue_node_t* node = impl->_nodes + index;
	for (;;)
	{
		st_queue_pointer_t free_list = impl->_free_list;
		node->_next._part._index = free_list._part._index;

		st_queue_pointer_t link;
		link._part._index = index;
		link._part._count = free_list._part._count + 1;
		if (impl->_free_list._atomic.compare_exchange_strong(free_list._entire, link._entire))
		{
			break;
		}
	}
}

static st_queue_node_t* _init_node(st_queue_impl_t* impl, uint32_t node_index)
{
	st_queue_node_t* node = impl->_nodes + node_index;
	node->_data = 0;
	node->_next._part._index = k_st_queue_invalid_index;
	node->_next._part._count = 0;

	std::atomic_thread_fence(std::memory_order_release);

	return node;
}
