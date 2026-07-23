#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/st_graphics_context.h>

#include <memory>

class st_gl_graphics_context : public st_graphics_context
{
public:

	st_gl_graphics_context(const class st_window* window);
	~st_gl_graphics_context();

	std::unique_ptr<st_device> create_device(const st_device_desc& desc) override;

	// Informational.
	e_st_graphics_api get_api() { return e_st_graphics_api::opengl; }
	void get_supported_formats(
		const class st_window* window,
		class st_device* device,
		std::vector<e_st_format>& formats) override;

private:

	HDC _device_context;
	HGLRC _gl_context;

	// Active draw state.
	const class st_gl_shader* _bound_shader = nullptr;
	const struct st_gl_pipeline* _bound_pipeline = nullptr;
};

#endif
