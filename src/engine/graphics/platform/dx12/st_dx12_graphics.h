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
	~st_dx12_buffer_view();

	st_dx12_descriptor _handle;
	class st_dx12_descriptor_heap* _heap;
};

struct st_dx12_framebuffer : public st_framebuffer
{
	~st_dx12_framebuffer()
	{
		_targets.clear();
		_views.clear();
	}

	std::vector<st_texture*> _targets;
	std::vector<st_texture_view*> _views;
	st_texture* _depth_stencil = nullptr;
	st_texture_view* _depth_stencil_view = nullptr;
};

struct st_dx12_pipeline : public st_pipeline
{
	~st_dx12_pipeline() { _pipeline = nullptr; }

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipeline = nullptr;

	uint8_t _stencil_ref = 0;
};

struct st_dx12_render_pass : public st_render_pass
{
	st_viewport _viewport;
	std::vector<st_attachment_desc> _color_attachments;
	st_attachment_desc _depth_attachment;
};

struct st_dx12_resource_table : public st_resource_table
{
	std::vector<st_dx12_descriptor> _cbvs;
	std::vector<st_dx12_descriptor> _srvs;
	std::vector<st_dx12_descriptor> _uavs;
	std::vector<st_dx12_descriptor> _samplers;
	std::vector<st_dx12_descriptor> _buffers;
};

struct st_dx12_sampler : public st_sampler
{
	~st_dx12_sampler();

	st_dx12_descriptor _handle;
	class st_dx12_descriptor_heap* _heap;
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
		_cs = nullptr;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> _vs;
	Microsoft::WRL::ComPtr<ID3DBlob> _ps;
	Microsoft::WRL::ComPtr<ID3DBlob> _ds;
	Microsoft::WRL::ComPtr<ID3DBlob> _hs;
	Microsoft::WRL::ComPtr<ID3DBlob> _gs;
	Microsoft::WRL::ComPtr<ID3DBlob> _cs;

	uint8_t _type = 0;
};

struct st_dx12_swap_chain : public st_swap_chain
{
	~st_dx12_swap_chain()
	{
		_backbuffer_views.clear();
		_backbuffers.clear();

		_swap_chain_1 = nullptr;
		_swap_chain_2 = nullptr;
		_swap_chain_3 = nullptr;
	}

	Microsoft::WRL::ComPtr<IDXGISwapChain1> _swap_chain_1;
	Microsoft::WRL::ComPtr<IDXGISwapChain2> _swap_chain_2;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> _swap_chain_3;

	std::vector<std::unique_ptr<st_texture>> _backbuffers;
	std::vector<std::unique_ptr<st_texture_view>> _backbuffer_views;
};

struct st_dx12_texture : public st_texture
{
	~st_dx12_texture() { _handle = nullptr; }

	Microsoft::WRL::ComPtr<ID3D12Resource> _handle;

	uint32_t _width;
	uint32_t _height;
	uint32_t _depth;
	uint32_t _levels = 1;
	e_st_format _format;
	e_st_texture_usage_flags _usage = static_cast<e_st_texture_usage_flags>(0);
	e_st_texture_state _state = st_texture_state_common;
};

struct st_dx12_texture_view : public st_texture_view
{
	~st_dx12_texture_view();

	st_dx12_descriptor _handle;
	class st_dx12_descriptor_heap* _heap;
};

struct st_dx12_vertex_format : public st_vertex_format
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> _elements;
	D3D12_INPUT_LAYOUT_DESC _input_layout = {};
};
