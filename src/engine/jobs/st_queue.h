#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

/*
** Thread-safe, lock-free queue.
** https://www.research.ibm.com/people/m/michael/podc-1996.pdf
*/
class st_queue
{
public:
	st_queue(int node_count);
	~st_queue();

	void push(void* data);
	bool pop(void** data);

	int get_count() const;

private:
	void* _impl;
};
