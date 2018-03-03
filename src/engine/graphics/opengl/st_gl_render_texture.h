#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <cstdint>
#include <memory>

class st_gl_render_texture
{
public:
	st_gl_render_texture(uint32_t width, uint32_t height, e_st_texture_format format);
	~st_gl_render_texture();

	class st_texture* get_texture() const { return _texture.get(); }

private:
	std::unique_ptr<class st_texture> _texture;
};

#endif
