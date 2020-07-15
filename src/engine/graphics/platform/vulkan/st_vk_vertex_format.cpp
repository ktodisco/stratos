/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_vertex_format.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/geometry/st_vertex_attribute.h>

st_vk_vertex_format::st_vk_vertex_format()
{
}

st_vk_vertex_format::~st_vk_vertex_format()
{
}

void st_vk_vertex_format::platform_finalize()
{
	// Create the element descriptions.
	size_t offset = 0;
	for (uint32_t itr = 0; itr < get_attribute_count(); ++itr)
	{
		const st_vertex_attribute* attr = &get_attribute(itr);
		int32_t size = 0;
		size_t data_size = sizeof(float);
		e_st_format format = st_format_r32g32b32_float;
		vk::VertexInputRate input_rate = vk::VertexInputRate::eVertex;

		switch (attr->_type)
		{
		case st_vertex_attribute_position:
			size = 3;
			break;
		case st_vertex_attribute_normal:
			size = 3;
			break;
		case st_vertex_attribute_binormal:
			size = 3;
			break;
		case st_vertex_attribute_tangent:
			size = 3;
			break;
		case st_vertex_attribute_joints:
			format = st_format_r32g32b32a32_uint;
			data_size = sizeof(uint32_t);
			size = 4;
			break;
		case st_vertex_attribute_color:
			format = st_format_r32g32b32a32_float;
			size = 4;
			break;
		case st_vertex_attribute_weights:
			format = st_format_r32g32b32a32_float;
			size = 4;
			break;
		case st_vertex_attribute_uv:
			format = st_format_r32g32_float;
			size = 2;
			break;
		default:
			assert(false);
			break;
		}

		vk::VertexInputBindingDescription binding = vk::VertexInputBindingDescription()
			.setBinding(itr)
			.setInputRate(input_rate)
			.setStride(size * data_size);

		vk::VertexInputAttributeDescription attribute = vk::VertexInputAttributeDescription()
			.setBinding(itr)
			.setFormat(vk::Format(format))
			.setLocation(attr->_unit)
			.setOffset(offset);

		_binding_descs.push_back(binding);
		_attribute_descs.push_back(attribute);

		offset += size * data_size;
	}

	_input_layout
		.setVertexBindingDescriptionCount(_binding_descs.size())
		.setVertexAttributeDescriptionCount(_attribute_descs.size())
		.setPVertexBindingDescriptions(_binding_descs.data())
		.setPVertexAttributeDescriptions(_attribute_descs.data());
}

#endif
