/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_command_queue.h>

#if defined(ST_GRAPHICS_API_OPENGL)

st_gl_command_queue::st_gl_command_queue(HDC context)
	: _device_context(context)
{
}

st_gl_command_queue::~st_gl_command_queue()
{
}

void st_gl_command_queue::signal(st_fence* fence_, uint64_t value)
{
	st_gl_fence* fence = static_cast<st_gl_fence*>(fence_);

	if (fence->_sync != nullptr)
	{
		glDeleteSync(fence->_sync);
	}

	fence->_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void st_gl_command_queue::wait(st_fence* fence_, uint64_t value)
{
	st_gl_fence* fence = static_cast<st_gl_fence*>(fence_);

	glWaitSync(fence->_sync, 0, GL_TIMEOUT_IGNORED);
}

void st_gl_command_queue::execute(st_command_list* command_list)
{
}

void st_gl_command_queue::present(st_swap_chain* swap_chain, uint32_t index)
{
	SwapBuffers(_device_context);
}

#endif
