/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_condvar.h"

st_condvar::st_condvar()
{
}

st_condvar::~st_condvar()
{
}

void st_condvar::wait()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_condvar.wait(lock);
}

void st_condvar::wait_for(int ms)
{
	std::unique_lock<std::mutex> lock(_mutex);
	_condvar.wait_for(lock, std::chrono::milliseconds(ms));
}

void st_condvar::wake_all()
{
	_condvar.notify_all();
}
