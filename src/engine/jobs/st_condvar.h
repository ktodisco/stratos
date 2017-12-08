#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <condition_variable>
#include <mutex>

/*
** Condition variable object.
*/
class st_condvar
{
public:
	st_condvar();
	~st_condvar();

	void wait();
	void wait_for(int ms);
	void wake_all();

private:
	std::condition_variable _condvar;
	std::mutex _mutex;
};
