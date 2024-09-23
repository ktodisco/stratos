#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

#include <cstdint>
#include <dxgi1_6.h>
#include <graphics/dds.h>
#include <graphics/st_graphics.h>

#include <memory>

namespace st_texture_loader
{
	std::unique_ptr<st_texture> load(const char* filename);
	std::unique_ptr<st_texture> load_stb_texture(const char* fullpath);
	std::unique_ptr<st_texture> load_dds_texture(const char* fullpath);

	#define ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )
	inline e_st_format get_st_format(const DirectX::DDS_PIXELFORMAT& ddpf)
	{
		if (ddpf.flags & DDS_RGB)
		{
			// Note that sRGB formats are written using the "DX10" extended header

			switch (ddpf.RGBBitCount)
			{
			case 32:
				if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				{
					return st_format_r8g8b8a8_unorm;
				}

				if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
				{
					return st_format_b8g8r8a8_unorm;
				}

				if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
				{
					return st_format_b8g8r8x8_unorm;
				}

				// No DXGI format maps to ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) aka D3DFMT_X8B8G8R8

				// Note that many common DDS reader/writers (including D3DX) swap the
				// the RED/BLUE masks for 10:10:10:2 formats. We assume
				// below that the 'backwards' header mask is being used since it is most
				// likely written by D3DX. The more robust solution is to use the 'DX10'
				// header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

				// For 'correct' writers, this should be 0x000003ff,0x000ffc00,0x3ff00000 for RGB data
				if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
				{
					return st_format_r10g10b10a2_unorm;
				}

				// No DXGI format maps to ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) aka D3DFMT_A2R10G10B10

				if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
				{
					return st_format_r16g16_unorm;
				}

				if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
				{
					// Only 32-bit color channel format in D3D9 was R32F
					return st_format_r32_float; // D3DX writes this out as a FourCC of 114
				}
				break;

			case 24:
				// No 24bpp DXGI formats aka D3DFMT_R8G8B8
				break;

			case 16:
				if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
				{
					return st_format_b5g5r5a1_unorm;
				}
				if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0x0000))
				{
					return st_format_b5g6r5_unorm;
				}

				// No DXGI format maps to ISBITMASK(0x7c00,0x03e0,0x001f,0x0000) aka D3DFMT_X1R5G5B5

				if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
				{
					return st_format_b4g4r4a4_unorm;
				}

				// No DXGI format maps to ISBITMASK(0x0f00,0x00f0,0x000f,0x0000) aka D3DFMT_X4R4G4B4

				// No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
				break;
			}
		}
		else if (ddpf.flags & DDS_LUMINANCE)
		{
			if (8 == ddpf.RGBBitCount)
			{
				if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
				{
					return st_format_r8_unorm; // D3DX10/11 writes this out as DX10 extension
				}

				// No DXGI format maps to ISBITMASK(0x0f,0x00,0x00,0xf0) aka D3DFMT_A4L4

				if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
				{
					return st_format_r8g8_unorm; // Some DDS writers assume the bitcount should be 8 instead of 16
				}
			}

			if (16 == ddpf.RGBBitCount)
			{
				if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
				{
					return st_format_r16_unorm; // D3DX10/11 writes this out as DX10 extension
				}
				if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
				{
					return st_format_r8g8_unorm; // D3DX10/11 writes this out as DX10 extension
				}
			}
		}
		else if (ddpf.flags & DDS_ALPHA)
		{
			if (8 == ddpf.RGBBitCount)
			{
				return st_format_a8_unorm;
			}
		}
		else if (ddpf.flags & DDS_BUMPDUDV)
		{
			if (16 == ddpf.RGBBitCount)
			{
				if (ISBITMASK(0x00ff, 0xff00, 0x0000, 0x0000))
				{
					return st_format_r8g8_snorm; // D3DX10/11 writes this out as DX10 extension
				}
			}

			if (32 == ddpf.RGBBitCount)
			{
				if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				{
					return st_format_r8g8b8a8_snorm; // D3DX10/11 writes this out as DX10 extension
				}
				if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
				{
					return st_format_r16g16_snorm; // D3DX10/11 writes this out as DX10 extension
				}

				// No DXGI format maps to ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000) aka D3DFMT_A2W10V10U10
			}
		}
		else if (ddpf.flags & DDS_FOURCC)
		{
			if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
			{
				return st_format_bc1_unorm;
			}
			if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
			{
				return st_format_bc2_unorm;
			}
			if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
			{
				return st_format_bc3_unorm;
			}

			// While pre-multiplied alpha isn't directly supported by the DXGI formats,
			// they are basically the same as these BC formats so they can be mapped
			if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
			{
				return st_format_bc2_unorm;
			}
			if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
			{
				return st_format_bc3_unorm;
			}

			if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC)
			{
				return st_format_bc4_unorm;
			}
			if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC)
			{
				return st_format_bc4_unorm;
			}
			if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC)
			{
				return st_format_bc4_snorm;
			}

			if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC)
			{
				return st_format_bc5_unorm;
			}
			if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC)
			{
				return st_format_bc5_unorm;
			}
			if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC)
			{
				return st_format_bc5_snorm;
			}

			// BC6H and BC7 are written using the "DX10" extended header

			if (MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.fourCC)
			{
				return st_format_r8g8_b8g8_unorm;
			}
			if (MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.fourCC)
			{
				return st_format_g8r8_g8b8_unorm;
			}

			if (MAKEFOURCC('Y', 'U', 'Y', '2') == ddpf.fourCC)
			{
				return st_format_yuy2;
			}

			// Check for D3DFORMAT enums being set here
			switch (ddpf.fourCC)
			{
			case 36: // D3DFMT_A16B16G16R16
				return st_format_r16g16b16a16_unorm;

			case 110: // D3DFMT_Q16W16V16U16
				return st_format_r16g16b16a16_snorm;

			case 111: // D3DFMT_R16F
				return st_format_r16_float;

			case 112: // D3DFMT_G16R16F
				return st_format_r16g16_float;

			case 113: // D3DFMT_A16B16G16R16F
				return st_format_r16g16b16a16_float;

			case 114: // D3DFMT_R32F
				return st_format_r32_float;

			case 115: // D3DFMT_G32R32F
				return st_format_r32g32_float;

			case 116: // D3DFMT_A32B32G32R32F
				return st_format_r32g32b32a32_float;
			}
		}

		return st_format_unknown;
	}
	#undef ISBITMASK
}
