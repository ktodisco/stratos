#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <vector>

class st_graphics_context
{
public:
	virtual ~st_graphics_context() {}

	virtual std::unique_ptr<st_device> create_device(const st_device_desc& desc) = 0;

	// Informational.
	virtual e_st_graphics_api get_api() = 0;
	virtual void get_supported_formats(
		const class st_window* window,
		class st_device* device,
		std::vector<e_st_format>& formats) = 0;

	static std::unique_ptr<st_graphics_context> create(e_st_graphics_api api, const class st_window* window);
	static st_graphics_context* get();

protected:
	uint32_t _frame_index = 0;
	static st_graphics_context* _this;
};