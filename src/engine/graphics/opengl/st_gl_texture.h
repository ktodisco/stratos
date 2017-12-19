#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <cstdint>

class st_gl_texture
{
public:
	st_gl_texture();
	~st_gl_texture();

	void load_from_data(uint32_t width, uint32_t height, uint32_t channels, void* data);
	bool load_from_file(const char* path);

	uint32_t get_handle() const { return _handle; }

private:
	uint32_t _handle;
};
