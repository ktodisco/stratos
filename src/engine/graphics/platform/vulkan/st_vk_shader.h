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
	st_vk_shader(const char* source, uint8_t type) {}
	~st_vk_shader() {}

	/*ID3DBlob* get_vertex_blob() const { return _vs.Get(); }
	ID3DBlob* get_pixel_blob() const { return _ps.Get(); }
	ID3DBlob* get_domain_blob() const { return _ds.Get(); }
	ID3DBlob* get_hull_blob() const { return _hs.Get(); }
	ID3DBlob* get_geometry_blob() const { return _gs.Get(); }*/

	bool has_vertex() const { return _type & st_shader_type_vertex; }
	bool has_pixel() const { return _type & st_shader_type_pixel; }
	bool has_domain() const { return _type & st_shader_type_domain; }
	bool has_hull() const { return _type & st_shader_type_hull; }
	bool has_geometry() const { return _type & st_shader_type_geometry; }

private:
	/*Microsoft::WRL::ComPtr<ID3DBlob> _vs;
	Microsoft::WRL::ComPtr<ID3DBlob> _ps;
	Microsoft::WRL::ComPtr<ID3DBlob> _ds;
	Microsoft::WRL::ComPtr<ID3DBlob> _hs;
	Microsoft::WRL::ComPtr<ID3DBlob> _gs;*/

	uint8_t _type = 0;
};

#endif