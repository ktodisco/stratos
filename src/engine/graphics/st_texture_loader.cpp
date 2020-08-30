/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_texture_loader.h>

#include <core/st_core.h>

#include <stb_image.h>

#include <cassert>
#include <iostream>
#include <string>

extern char g_root_path[256];

namespace st_texture_loader
{

std::unique_ptr<st_texture> load(const char* filename)
{
	std::string fullpath = g_root_path;
	fullpath += (filename && filename[0]) ? filename : "data/textures/default_albedo.png";

	auto ends_with = [&](const std::string& s1, const std::string& s2)
	{
		return s1.compare(s1.length() - s2.length(), s2.length(), s2) == 0;
	};

	std::unique_ptr<st_texture> ret;

	if (ends_with(fullpath, ".dds"))
	{
		ret = load_dds_texture(fullpath.c_str());
	}
	else
	{
		ret = load_stb_texture(fullpath.c_str());
	}

	if (ret == nullptr)
	{
		std::cerr << "Failed to load " << filename << std::endl;
		assert(false);
	}

	return std::move(ret);
}

std::unique_ptr<st_texture> load_stb_texture(const char* fullpath)
{
	int width, height, channels_in_file;
	uint8_t* data = stbi_load(fullpath, &width, &height, &channels_in_file, 4);
	if (!data)
	{
		return nullptr;
	}

	std::unique_ptr<st_texture> ret = std::make_unique<st_texture>(
		width,
		height,
		1,
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		data);

	stbi_image_free(data);

	return std::move(ret);
}

std::unique_ptr<st_texture> load_dds_texture(const char* fullpath)
{
	std::unique_ptr<uint8_t[]> dds_data;

	// open the file
	HANDLE hFile = CreateFile2(str_to_wstr(fullpath).c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		OPEN_EXISTING,
		nullptr);

	if (!hFile)
	{
		return nullptr;
	}

	// Get the file size
	FILE_STANDARD_INFO file_info;
	if (!GetFileInformationByHandleEx(hFile, FileStandardInfo, &file_info, sizeof(file_info)))
	{
		return nullptr;
	}

	// File is too big for 32-bit allocation, so reject read
	if (file_info.EndOfFile.HighPart > 0)
	{
		return nullptr;
	}

	// Need at least enough data to fill the header and magic number to be a valid DDS
	if (file_info.EndOfFile.LowPart < (sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER)))
	{
		return nullptr;
	}

	// create enough space for the file data
	dds_data.reset(new (std::nothrow) uint8_t[file_info.EndOfFile.LowPart]);
	if (!dds_data)
	{
		return nullptr;
	}

	// read the data in
	DWORD bytes_read = 0;
	if (!ReadFile(hFile,
		dds_data.get(),
		file_info.EndOfFile.LowPart,
		&bytes_read,
		nullptr
	))
	{
		return nullptr;
	}

	if (bytes_read < file_info.EndOfFile.LowPart)
	{
		return nullptr;
	}

	// DDS files always start with the same magic number ("DDS ")
	auto magic_number = *reinterpret_cast<const uint32_t*>(dds_data.get());
	if (magic_number != DirectX::DDS_MAGIC)
	{
		return nullptr;
	}

	auto header = reinterpret_cast<const DirectX::DDS_HEADER*>(dds_data.get() + sizeof(uint32_t));

	// Verify header to validate DDS file
	if (header->size != sizeof(DirectX::DDS_HEADER) ||
		header->ddspf.size != sizeof(DirectX::DDS_PIXELFORMAT))
	{
		return nullptr;
	}

	// Check for DX10 extension
	bool dxt10 = false;
	if ((header->ddspf.flags & DDS_FOURCC) &&
		(MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC))
	{
		// Must be long enough for both headers and magic value
		if (file_info.EndOfFile.LowPart < (sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER_DXT10)))
		{
			return nullptr;
		}

		dxt10 = true;
	}

	// setup the pointers in the process request
	auto offset = sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER)
		+ (dxt10 ? sizeof(DirectX::DDS_HEADER_DXT10) : 0u);
	auto bit_data = dds_data.get() + offset;

	CloseHandle(hFile);

	// CreateTextureFromDDS.
	auto texture = std::make_unique<st_texture>(
		header->width,
		header->height,
		header->mipMapCount,
		get_st_format(header->ddspf),
		e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		bit_data);

	return std::move(texture);
}

};
