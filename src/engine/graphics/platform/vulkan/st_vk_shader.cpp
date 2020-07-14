/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_shader.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>

#include <fstream>
#include <vector>

void load_shader(vk::Device* device, std::string file_name, vk::ShaderModule& shader)
{
	extern char g_root_path[256];
	std::string full_path = std::string(g_root_path) + std::string(file_name);

	std::ifstream shader_file(full_path, std::ios::binary);
	std::vector<unsigned char> shader_blob(std::istreambuf_iterator<char>(shader_file), {});

	vk::ShaderModuleCreateInfo create_info = vk::ShaderModuleCreateInfo()
		.setCodeSize(shader_blob.size())
		.setPCode(reinterpret_cast<uint32_t*>(shader_blob.data()));

	device->createShaderModule(&create_info, nullptr, &shader);
}

st_vk_shader::st_vk_shader(const char* source, uint8_t type)
	: _type(type)
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	if (type & st_shader_type_vertex)
	{
		load_shader(
			device,
			std::string(source) + std::string("_vert.spirv"),
			_vs);
	}

	if (type & st_shader_type_pixel)
	{
		load_shader(
			device,
			std::string(source) + std::string("_frag.spirv"),
			_ps);
	}
}

st_vk_shader::~st_vk_shader()
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	if (_type & st_shader_type_vertex)
		device->destroyShaderModule(_vs);
	if (_type & st_shader_type_pixel)
		device->destroyShaderModule(_ps);
	if (_type & st_shader_type_domain)
		device->destroyShaderModule(_ds);
	if (_type & st_shader_type_geometry)
		device->destroyShaderModule(_gs);
	if (_type & st_shader_type_hull)
		device->destroyShaderModule(_hs);
}

#endif
