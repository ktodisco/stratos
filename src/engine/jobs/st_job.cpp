/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_job.h"

#include "st_condvar.h"
#include "st_fiber.h"
#include "st_intpool.h"
#include "st_queue.h"

#include <atomic>
#include <thread>
#include <vector>

void* st_job::_impl = 0;

struct st_job_instance_t
{
	st_job_instance_t() {}

	st_job_decl_t* _decl;

	int32_t* _waiting_count;

	int _pool_index;

	st_fiber _fiber;
	st_fiber* _parent_fiber;
};

struct st_job_system_impl_t
{
	st_job_system_impl_t(int queue_size, int fiber_count) :
		_main_thread(std::this_thread::get_id()),
		_job_queue(queue_size),
		_job_instance_pool(fiber_count),
		_wait_queue(queue_size)
	{}

	std::thread::id _main_thread;

	st_queue _job_queue;

	st_intpool _job_instance_pool;
	st_job_instance_t* _job_instance_data;

	st_queue _wait_queue;

	std::vector<std::thread*> _worker_threads;

	st_condvar _work_added;
	st_condvar _work_exhausted;

	bool _terminate;
};

static int _st_job_instance_thread_worker(void* data);
static bool _st_job_schedule(st_job_system_impl_t* impl, st_fiber* parent_fiber);
static void _st_job_run(st_job_system_impl_t* impl, st_fiber* parent_fiber, st_job_instance_t* job);
static void _st_job_fiber_worker(void* data);

void st_job::startup(
	uint32_t hardware_thread_mask,
	int queue_size,
	int fiber_count)
{
	st_job_system_impl_t* impl = new st_job_system_impl_t(queue_size, fiber_count);

	impl->_terminate = false;

	impl->_job_instance_data = new st_job_instance_t[fiber_count];
	for (int i = 0; i < fiber_count; ++i)
	{
		st_job_instance_t* instance = &impl->_job_instance_data[i];
		instance->_fiber = st_fiber(_st_job_fiber_worker, instance, 64 * 1024);
		instance->_pool_index = i;
	}

	int hardware_thread_count = std::thread::hardware_concurrency();
	for (int i = 0; i < hardware_thread_count; ++i)
	{
		if ((hardware_thread_mask & (1 << i)) != 0)
		{
			impl->_worker_threads.push_back(new std::thread(_st_job_instance_thread_worker, impl));
		}
	}

	_impl = impl;
}

void st_job::shutdown()
{
	st_job_system_impl_t* impl = static_cast<st_job_system_impl_t*>(_impl);

	impl->_terminate = true;
	impl->_work_added.wake_all();
	for (auto& t : impl->_worker_threads)
	{
		t->join();
		delete t;
	}

	delete[] impl->_job_instance_data;
}

void st_job::run(st_job_decl_t* decls, int decl_count, int32_t* counter)
{
	*counter = decl_count;

	st_job_system_impl_t* impl = static_cast<st_job_system_impl_t*>(_impl);
	for (int i = 0; i < decl_count; ++i)
	{
		decls[i]._pending_count = counter;
		impl->_job_queue.push(decls + i);
	}

	impl->_work_added.wake_all();
}

void st_job::wait(int32_t* counter)
{
	if (*counter > 0)
	{
		/*
		** If we're not the main thread, assume we're waiting from within a job.
		** In this case, push the current job onto the wait list, and reschedule.
		*/
		st_job_system_impl_t* impl = static_cast<st_job_system_impl_t*>(_impl);
		if (std::this_thread::get_id() != impl->_main_thread)
		{
			st_job_instance_t* job = static_cast<st_job_instance_t*>(st_fiber::get_data());
			job->_waiting_count = counter;
			impl->_wait_queue.push(job);

			st_fiber::switch_to(*job->_parent_fiber);
		}
		/*
		** Otherwise, in the main thread, block until jobs are complete.
		*/
		else
		{
			while (*reinterpret_cast<std::atomic_int*>(counter) > 0)
			{
				impl->_work_exhausted.wait();
			}
		}
	}
}

static int _st_job_instance_thread_worker(void* data)
{
	st_job_system_impl_t* impl = static_cast<st_job_system_impl_t*>(data);

	st_fiber parent_fiber = st_fiber::convert_thread(0);

	while (!impl->_terminate)
	{
		if (!_st_job_schedule(impl, &parent_fiber))
		{
			impl->_work_exhausted.wake_all();
			impl->_work_added.wait_for(1);
		}
	}

	return 0;
}

static bool _st_job_schedule(st_job_system_impl_t* impl, st_fiber* parent_fiber)
{
	/* Check for waiting jobs that are ready to run. */
	st_job_instance_t* job;
	int retry_wait_count = impl->_wait_queue.get_count();
	while (retry_wait_count-- > 0 && impl->_wait_queue.pop((void**)&job))
	{
		if (job->_waiting_count == 0 || job->_waiting_count[0] == 0)
		{
			_st_job_run(impl, parent_fiber, job);
			return true;
		}
		else
		{
			impl->_wait_queue.push(job);
		}
	}

	/* Look for queued jobs. */
	st_job_decl_t* decl;
	if (impl->_job_queue.pop((void**)&decl))
	{
		int st_job_index = impl->_job_instance_pool.alloc();

		job = &impl->_job_instance_data[st_job_index];
		job->_decl = decl;
		job->_pool_index = st_job_index;

		_st_job_run(impl, parent_fiber, job);

		return true;
	}

	return impl->_wait_queue.get_count() != 0;
}

static void _st_job_run(st_job_system_impl_t* impl, st_fiber* parent_fiber, st_job_instance_t* job)
{
	job->_parent_fiber = parent_fiber;
	job->_waiting_count = 0;

	st_fiber::switch_to(job->_fiber);

	if (job->_waiting_count == 0 || job->_waiting_count[0] == 0)
	{
		impl->_job_instance_pool.free(job->_pool_index);

		(*reinterpret_cast<std::atomic_int*>(job->_decl->_pending_count))--;
	}
}

static void _st_job_fiber_worker(void* data)
{
	for (;;)
	{
		st_job_instance_t* job = static_cast<st_job_instance_t*>(st_fiber::get_data());
		job->_decl->_entry(job->_decl->_data);
		st_fiber::switch_to(*job->_parent_fiber);
	}
}
