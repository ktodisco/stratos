#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

/*
** A thread-safe and lock-free pool of integers.
*/
class st_intpool
{
public:
	st_intpool(int index_count);
	~st_intpool();

	int alloc();
	void free(int index);

	int get_index_count() const;

private:
	void* _impl;
};
