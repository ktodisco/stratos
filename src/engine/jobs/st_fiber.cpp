/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_fiber.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

st_fiber::st_fiber(function_t func, void* func_data, size_t stack_size)
{
	const size_t k_stack_align = 64 * 1024;
	stack_size = stack_size > k_stack_align ? stack_size : k_stack_align;
	stack_size = (stack_size + k_stack_align - 1) & ~(k_stack_align - 1);

	_impl = CreateFiber(stack_size, (LPFIBER_START_ROUTINE)func, func_data);
}

st_fiber::~st_fiber()
{
	if (_impl)
	{
		DeleteFiber(_impl);
	}
}

st_fiber& st_fiber::operator=(st_fiber&& other)
{
	if (&other != this)
	{
		_impl = other._impl;
		other._impl = 0;
	}
	return *this;
}

st_fiber st_fiber::convert_thread(void* data)
{
	st_fiber fiber;
	fiber._impl = ConvertThreadToFiber(data);
	return fiber;
}

void st_fiber::switch_to(const st_fiber& fiber)
{
	SwitchToFiber(fiber._impl);
}

void* st_fiber::get_data()
{
	return GetFiberData();
}
