/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/dx12/st_dx12_vertex_format.h>

#include <graphics/st_vertex_attribute.h>

#include <cassert>

st_dx12_vertex_format::st_dx12_vertex_format()
{
}

st_dx12_vertex_format::~st_dx12_vertex_format()
{
}

void st_dx12_vertex_format::platform_finalize()
{
	// Create the element descriptions.
	size_t offset = 0;
	for (uint32_t itr = 0; itr < get_attribute_count(); ++itr)
	{
		const st_vertex_attribute* attr = &get_attribute(itr);
		int32_t size = 0;
		size_t data_size = sizeof(float);
		LPCSTR semantic_name;
		UINT semantic_index = 0;
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32_FLOAT;
		UINT input_slot;
		UINT aligned_byte_offset = 0;
		D3D12_INPUT_CLASSIFICATION classification = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

		switch (attr->_type)
		{
		case st_vertex_attribute_position:
			semantic_name = "POSITION";
			size = 3;
			break;
		case st_vertex_attribute_normal:
			semantic_name = "NORMAL";
			size = 3;
			break;
		case st_vertex_attribute_binormal:
			semantic_name = "BINORMAL";
			size = 3;
			break;
		case st_vertex_attribute_tangent:
			semantic_name = "TANGENT";
			size = 3;
			break;
		case st_vertex_attribute_joints:
			semantic_name = "JOINTS";
			format = DXGI_FORMAT_R32G32B32A32_UINT;
			data_size = sizeof(uint32_t);
			size = 4;
			break;
		case st_vertex_attribute_color:
			semantic_name = "COLOR";
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			size = 4;
			break;
		case st_vertex_attribute_weights:
			semantic_name = "WEIGHT";
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			size = 4;
			break;
		case st_vertex_attribute_uv:
			semantic_name = "UV";
			format = DXGI_FORMAT_R32G32_FLOAT;
			size = 2;
			break;
		default:
			assert(false);
			break;
		}

		_elements.push_back({
			semantic_name,
			0,
			format,
			0,
			(UINT)offset,
			classification,
			0
		});

		offset += size * data_size;
	}

	_input_layout.NumElements = get_attribute_count();
	_input_layout.pInputElementDescs = &_elements[0];
}