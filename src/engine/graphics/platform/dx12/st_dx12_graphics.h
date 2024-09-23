#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <pix3.h>

#include <graphics/platform/dx12/d3dx12.h>

#include <memory>
#include <vector>

#include <Windows.h>
#include <wrl.h>

#if defined(_DEBUG)
#define ST_NAME_DX12_OBJECT(object, name) (object)->SetName((LPCWSTR)name)
#else
#define ST_NAME_DX12_OBJECT(object, name)
#endif

typedef uint32_t st_dx12_descriptor;

struct st_dx12_buffer : public st_buffer
{
	~st_dx12_buffer() { _buffer = nullptr; }

	uint32_t _count;
	e_st_buffer_usage_flags _usage;
	size_t _element_size;
	Microsoft::WRL::ComPtr<ID3D12Resource> _buffer;
};

struct st_dx12_buffer_view : public st_buffer_view
{
	union
	{
		D3D12_VERTEX_BUFFER_VIEW vertex;
		D3D12_INDEX_BUFFER_VIEW index;
	};
};

struct st_dx12_geometry : public st_geometry
{
	std::unique_ptr<st_buffer> _vertex_buffer;
	std::unique_ptr<st_buffer> _index_buffer;

	std::unique_ptr<st_buffer_view> _vertex_buffer_view;
	std::unique_ptr<st_buffer_view> _index_buffer_view;

	uint32_t _index_count = 0;
};

struct st_dx12_pipeline : public st_pipeline
{
	~st_dx12_pipeline() { _pipeline = nullptr; }

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipeline;
};

struct st_dx12_render_pass : public st_render_pass
{
	std::unique_ptr<class st_dx12_framebuffer> _framebuffer;
	st_viewport _viewport;
};

struct st_dx12_resource_table : public st_resource_table
{
	std::vector<st_dx12_descriptor> _cbvs;
	std::vector<st_dx12_descriptor> _srvs;
	std::vector<st_dx12_descriptor> _samplers;
	std::vector<st_dx12_descriptor> _buffers;
};

struct st_dx12_shader : public st_shader
{
	~st_dx12_shader()
	{
		_vs = nullptr;
		_ps = nullptr;
		_ds = nullptr;
		_hs = nullptr;
		_gs = nullptr;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> _vs;
	Microsoft::WRL::ComPtr<ID3DBlob> _ps;
	Microsoft::WRL::ComPtr<ID3DBlob> _ds;
	Microsoft::WRL::ComPtr<ID3DBlob> _hs;
	Microsoft::WRL::ComPtr<ID3DBlob> _gs;

	uint8_t _type = 0;
};

struct st_dx12_texture : public st_texture
{
	~st_dx12_texture() { _handle = nullptr; }

	Microsoft::WRL::ComPtr<ID3D12Resource> _handle;

	uint32_t _width;
	uint32_t _height;
	uint32_t _levels = 1;
	e_st_format _format;
	e_st_texture_usage_flags _usage = static_cast<e_st_texture_usage_flags>(0);
	e_st_texture_state _state = st_texture_state_common;
};

struct st_dx12_texture_view : public st_texture_view
{
	st_dx12_descriptor _handle;
};

struct st_dx12_vertex_format : public st_vertex_format
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> _elements;
	D3D12_INPUT_LAYOUT_DESC _input_layout = {};
};
