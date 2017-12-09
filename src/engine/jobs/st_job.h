#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <cstdint>

/*
** Job entry point.
*/
typedef void(*st_job_function_t)(void* data);

/*
** Defines a job.
*/
struct st_job_decl_t
{
	st_job_function_t _entry;
	void* _data;

	int32_t* _pending_count;
};

/*
** Job system functionality.
*/
class st_job
{
public:
	static void startup(
		uint32_t hardware_thread_mask,
		int queue_size,
		int fiber_count);

	static void shutdown();

	static void run(st_job_decl_t* decls, int decl_count, int32_t* counter);

	static void wait(int32_t* counter);

private:
	static void* _impl;
};
