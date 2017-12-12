#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <Windows.h>

class st_render_context
{
public:

	st_render_context(HDC device_context);
	~st_render_context();

	HDC get_device_context() const
	{
		return _device_context;
	}

	void acquire();
	void release();

private:

	HDC _device_context;
	HGLRC _gl_context;
};