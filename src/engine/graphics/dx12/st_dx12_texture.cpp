/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/dx12/st_dx12_texture.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <core/st_core.h>

#include <graphics/dx12/st_dx12_render_context.h>

#include <stb_image.h>
#include <string>

st_dx12_texture::st_dx12_texture()
{
}

st_dx12_texture::~st_dx12_texture()
{
}

void st_dx12_texture::load_from_data(uint32_t width, uint32_t height, e_st_texture_format format, void* data)
{
	_format = format;

	st_dx12_render_context::get()->create_texture(
		width,
		height,
		format,
		data,
		_handle.GetAddressOf(),
		&_sampler,
		&_srv);
}

bool st_dx12_texture::load_from_file(const char* path)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += (path && path[0]) ? path : "data/textures/default_albedo.png";

	int width, height, channels_in_file;
	uint8_t* data = stbi_load(fullpath.c_str(), &width, &height, &channels_in_file, 4);
	if (!data)
	{
		return false;
	}

	load_from_data(width, height, st_texture_format_r8g8b8a8_unorm, data);

	stbi_image_free(data);

	return true;
}

void st_dx12_texture::bind(class st_dx12_render_context* context)
{
	context->set_shader_resource_table(_srv);
	context->set_sampler_table(_sampler);
}

void st_dx12_texture::set_name(std::string name)
{
	ST_NAME_DX12_OBJECT(_handle.Get(), str_to_wstr(name).c_str());
}

#endif
