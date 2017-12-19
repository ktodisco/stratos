#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/st_compiler_defines.h"

#if defined(ST_MINGW)
#include <sys/types.h>
#endif

/*
** A fiber object.
** This the execution context for a thread including the registers and stack.
*/
class st_fiber
{
public:
	typedef void(*function_t)(void* data);

	st_fiber() : _impl(0) {}
	st_fiber(st_fiber&& other) : _impl(other._impl) { other._impl = 0; }
	st_fiber(function_t func, void* func_data, size_t stack_size);
	~st_fiber();

	st_fiber& operator=(st_fiber&& other);

	static st_fiber convert_thread(void* data);
	static void switch_to(const st_fiber& fiber);
	static void* get_data();

private:
	void* _impl;
};
