#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/st_shader_manager.h>

/*
** Represents a shader.
*/
class st_vk_shader
{
public:
	st_vk_shader(const char* source, uint8_t type);
	~st_vk_shader();

	const vk::ShaderModule& get_vertex_module() const { return _vs; }
	const vk::ShaderModule& get_pixel_module() const { return _ps; }
	const vk::ShaderModule& get_domain_module() const { return _ds; }
	const vk::ShaderModule& get_hull_module() const { return _hs; }
	const vk::ShaderModule& get_geometry_module() const { return _gs; }

	bool has_vertex() const { return _type & st_shader_type_vertex; }
	bool has_pixel() const { return _type & st_shader_type_pixel; }
	bool has_domain() const { return _type & st_shader_type_domain; }
	bool has_hull() const { return _type & st_shader_type_hull; }
	bool has_geometry() const { return _type & st_shader_type_geometry; }

private:
	vk::ShaderModule _vs;
	vk::ShaderModule _ps;
	vk::ShaderModule _ds;
	vk::ShaderModule _hs;
	vk::ShaderModule _gs;

	uint8_t _type = 0;
};

#endif
