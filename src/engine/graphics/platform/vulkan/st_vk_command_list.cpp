/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_command_list.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_command_allocator.h>
#include <graphics/platform/vulkan/st_vk_conversion.h>
#include <graphics/platform/vulkan/st_vk_device.h>

#include <graphics/st_drawcall.h>

st_vk_command_list::st_vk_command_list(const st_command_list_desc& desc, st_vk_device* device)
	: _device(device)
{
	st_vk_command_allocator* command_allocator = static_cast<st_vk_command_allocator*>(desc.allocator);
	_command_pool = command_allocator->get();

	vk::CommandBufferAllocateInfo command_buffer_info = vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(1)
		.setCommandPool(*_command_pool)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	VK_VALIDATE(_device->get()->allocateCommandBuffers(&command_buffer_info, &_command_buffer));

	// Create dynamic geometry buffers.
	const uint32_t k_dynamic_buffer_count = 10000;
	{
		st_buffer_desc desc;
		desc._count = k_dynamic_buffer_count;
		desc._element_size = sizeof(st_vk_procedural_vertex);
		desc._usage = e_st_buffer_usage::vertex | e_st_buffer_usage::transfer_dest;
		_dynamic_vertex_buffer = device->create_buffer(desc);
	}
	{
		st_buffer_desc desc;
		desc._count = k_dynamic_buffer_count;
		desc._element_size = sizeof(uint16_t);
		desc._usage = e_st_buffer_usage::index | e_st_buffer_usage::transfer_dest;
		_dynamic_index_buffer = device->create_buffer(desc);
	}
	{
		st_buffer_desc desc;
		desc._element_size = sizeof(uint8_t);
		desc._count = 128 * 1024 * 1024;
		desc._usage = e_st_buffer_usage::transfer_dest | e_st_buffer_usage::transfer_source;
		_upload_buffer = device->create_buffer(desc);
	}

	// Create the descriptor pool.
	std::vector<vk::DescriptorPoolSize> pool_sizes;
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(k_max_shader_resources));
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eSampledImage)
		.setDescriptorCount(k_max_shader_resources));
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eSampler)
		.setDescriptorCount(k_max_samplers));
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eStorageBuffer)
		.setDescriptorCount(k_max_shader_resources));

	vk::DescriptorPoolCreateInfo pool_info = vk::DescriptorPoolCreateInfo()
		.setMaxSets(1024)
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
		.setPoolSizeCount(pool_sizes.size())
		.setPPoolSizes(pool_sizes.data());

	VK_VALIDATE(_device->get()->createDescriptorPool(&pool_info, nullptr, &_descriptor_pool));

	for (uint32_t ds_itr = 0; ds_itr < k_max_frames; ++ds_itr)
	{
		// TODO: Arbitrary number. What's the best way to size this?
		_descriptor_set_pool[ds_itr].reserve(1024);
	}
}

st_vk_command_list::~st_vk_command_list()
{
	_dynamic_index_buffer = nullptr;
	_dynamic_vertex_buffer = nullptr;
	_upload_buffer = nullptr;

	for (uint32_t i = 0; i < _countof(_descriptor_set_pool); ++i)
	{
		_descriptor_set_pool[i].clear();
	}

	_device->get()->destroyDescriptorPool(_descriptor_pool, nullptr);

	_device->get()->freeCommandBuffers(*_command_pool, 1, &_command_buffer);
	_command_pool = nullptr;
}

void st_vk_command_list::begin(st_command_allocator* command_allocator_)
{
	vk::CommandBufferBeginInfo begin_info = vk::CommandBufferBeginInfo()
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	VK_VALIDATE(_command_buffer.begin(&begin_info));

	_device->map(_upload_buffer.get(), 0, { 0, 0 }, &_upload_buffer_head);

	_dynamic_vertex_bytes_written = 0;
	_dynamic_index_bytes_written = 0;
	_upload_buffer_offset = 0;

	// Reset the descriptor set pool.
	if (_descriptor_set_pool[_frame_index].size() > 0)
	{
		VK_VALIDATE(_device->get()->freeDescriptorSets(
			_descriptor_pool,
			_descriptor_set_pool[_frame_index].size(),
			_descriptor_set_pool[_frame_index].data()));
		_descriptor_set_pool[_frame_index].clear();
	}
}

void st_vk_command_list::end()
{
	_upload_buffer_offset = 0;
	_upload_buffer_head = nullptr;
	_device->unmap(_upload_buffer.get(), 0, { 0, 0 });

	VK_VALIDATE(_command_buffer.end());

	_frame_index = (_frame_index + 1) % k_max_frames;
}

void st_vk_command_list::set_pipeline(const st_pipeline* pipeline_)
{
	const st_vk_pipeline* pipeline = static_cast<const st_vk_pipeline*>(pipeline_);
	_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->_pipeline);
}

void st_vk_command_list::set_compute_pipeline(const st_pipeline* pipeline_)
{
	const st_vk_pipeline* pipeline = static_cast<const st_vk_pipeline*>(pipeline_);
	_command_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline->_pipeline);
}

void st_vk_command_list::set_viewport(const st_viewport& viewport)
{
	// Flip the Vulkan viewport to match the coordinate systems of OpenGL and DirectX.
	vk::Viewport view = vk::Viewport()
		.setWidth(viewport._width)
		.setHeight(-viewport._height)
		.setX(viewport._x)
		.setY(viewport._height - viewport._y)
		.setMinDepth(viewport._min_depth)
		.setMaxDepth(viewport._max_depth);
	_command_buffer.setViewportWithCount(1, &view);
}

void st_vk_command_list::set_scissor(int left, int top, int right, int bottom)
{
	vk::Rect2D scissor = vk::Rect2D()
		.setOffset({ left, top })
		.setExtent({ uint32_t(right - left), uint32_t(bottom - top) });
	_command_buffer.setScissorWithCount(1, &scissor);
}

void st_vk_command_list::set_blend_factor(float r, float g, float b, float a)
{
}

void st_vk_command_list::draw(const st_static_drawcall& drawcall)
{
	const st_vk_buffer* vertex_buffer = static_cast<const st_vk_buffer*>(drawcall._vertex_buffer);
	const st_vk_buffer* index_buffer = static_cast<const st_vk_buffer*>(drawcall._index_buffer);

	vk::DeviceSize offset = vk::DeviceSize(0);
	_command_buffer.bindVertexBuffers(0, 1, &vertex_buffer->_buffer, &offset);
	_command_buffer.bindIndexBuffer(index_buffer->_buffer, 0, vk::IndexType::eUint16);

	_command_buffer.drawIndexed(
		drawcall._index_count,
		1,
		drawcall._index_offset,
		drawcall._vertex_offset,
		0);
}

void st_vk_command_list::draw(const struct st_dynamic_drawcall& drawcall)
{
	// TODO: Dynamic buffer limit checking.

	uint8_t* buffer_begin;
	_device->map(_dynamic_vertex_buffer.get(), 0, { 0, 0 }, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_vertex_bytes_written;

	std::vector<st_vk_procedural_vertex> verts;
	verts.reserve(drawcall._positions.size());

	for (uint32_t vert_itr = 0; vert_itr < drawcall._positions.size(); ++vert_itr)
	{
		verts.push_back({ drawcall._positions[vert_itr], drawcall._colors[vert_itr] });
	}

	memcpy(buffer_begin, &verts[0], sizeof(st_vk_procedural_vertex) * verts.size());
	_device->unmap(_dynamic_vertex_buffer.get(), 0, { 0, 0 });

	_device->map(_dynamic_index_buffer.get(), 0, { 0, 0 }, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_index_bytes_written;

	memcpy(buffer_begin, &drawcall._indices[0], sizeof(uint16_t) * drawcall._indices.size());
	_device->unmap(_dynamic_index_buffer.get(), 0, { 0, 0 });

	st_vk_buffer* dynamic_vertex_buffer = static_cast<st_vk_buffer*>(_dynamic_vertex_buffer.get());
	st_vk_buffer* dynamic_index_buffer = static_cast<st_vk_buffer*>(_dynamic_index_buffer.get());

	vk::DeviceSize offset = vk::DeviceSize(0);
	_command_buffer.bindVertexBuffers(0, 1, &dynamic_vertex_buffer->_buffer, &offset);
	_command_buffer.bindIndexBuffer(dynamic_index_buffer->_buffer, 0, vk::IndexType::eUint16);

	_command_buffer.drawIndexed(
		drawcall._indices.size(),
		1,
		(_dynamic_index_bytes_written / sizeof(uint16_t)),
		(_dynamic_vertex_bytes_written / sizeof(st_vk_procedural_vertex)),
		0);

	_dynamic_vertex_bytes_written += sizeof(st_vk_procedural_vertex) * verts.size();
	_dynamic_index_bytes_written += sizeof(uint16_t) * drawcall._indices.size();
}

void st_vk_command_list::dispatch(const st_dispatch_args& args)
{
	_command_buffer.dispatch(args.group_count_x, args.group_count_y, args.group_count_z);
}

void st_vk_command_list::begin_marker(const std::string& marker)
{
	if (_device->has_markers())
	{
		vk::DebugMarkerMarkerInfoEXT marker_info = vk::DebugMarkerMarkerInfoEXT()
			.setPMarkerName(marker.c_str());

		_command_buffer.debugMarkerBeginEXT(&marker_info);
	}
}

void st_vk_command_list::end_marker()
{
	if (_device->has_markers())
	{
		_command_buffer.debugMarkerEndEXT();
	}
}

void st_vk_command_list::upload(st_texture* texture_, void* data)
{
	st_vk_texture* texture = static_cast<st_vk_texture*>(texture_);

	st_texture_desc desc;
	_device->get_desc(texture, &desc);

	std::vector<vk::BufferImageCopy> regions;
	uint64_t offset = 0;
	for (int i = 0; i < desc._levels; ++i)
	{
		uint32_t level_width = desc._width >> i;
		uint32_t level_height = desc._height >> i;

		// Stop on mips less than size 4.
		// TODO: This should be mips less than the texel block size of the compressed format.
		if (is_compressed(desc._format) &&
			(level_width < 4 || level_height < 4))
			break;

		size_t bpp = bits_per_pixel(desc._format);
		size_t num_bytes;
		get_surface_info(
			level_width,
			level_height,
			desc._format,
			&num_bytes,
			nullptr,
			nullptr);

		// Align the upload buffer offset to the pixel size to satisfy Vulkan requirement.
		_upload_buffer_offset = align_value(_upload_buffer_offset, bpp);

		memcpy(
			reinterpret_cast<uint8_t*>(_upload_buffer_head) + _upload_buffer_offset,
			reinterpret_cast<uint8_t*>(data) + offset,
			num_bytes);

		vk::ImageSubresourceLayers subresource = vk::ImageSubresourceLayers()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setLayerCount(1)
			.setMipLevel(i);

		vk::BufferImageCopy region = vk::BufferImageCopy()
			.setBufferImageHeight(level_height)
			.setBufferRowLength(level_width)
			.setBufferOffset(_upload_buffer_offset)
			.setImageExtent(vk::Extent3D(level_width, level_height, 1))
			.setImageOffset(vk::Offset3D(0, 0, 0))
			.setImageSubresource(subresource);

		regions.push_back(region);

		offset += num_bytes;
		_upload_buffer_offset += num_bytes;
	}

	st_vk_buffer* upload_buffer = static_cast<st_vk_buffer*>(_upload_buffer.get());

	vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor;

	if (desc._format == st_format_d16_unorm ||
		desc._format == st_format_d32_float)
	{
		aspect = vk::ImageAspectFlagBits::eDepth;
	}
	else if (desc._format == st_format_d24_unorm_s8_uint)
	{
		aspect = vk::ImageAspectFlagBits::eDepth |
			vk::ImageAspectFlagBits::eStencil;
	}

	vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
		.setAspectMask(aspect)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setBaseMipLevel(0)
		.setLevelCount(desc._levels);
	vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier()
		.setImage(texture->_handle)
		.setOldLayout(vk::ImageLayout::eUndefined)
		.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
		.setSubresourceRange(range);

	_command_buffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eTransfer,
		vk::DependencyFlags(),
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);

	_command_buffer.copyBufferToImage(
		upload_buffer->_buffer,
		texture->_handle,
		vk::ImageLayout::eTransferDstOptimal,
		regions.size(),
		regions.data());

	barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
	barrier.setNewLayout(convert_resource_state(texture->_initial_state));

	_command_buffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eTransfer,
		vk::DependencyFlags(),
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);
}

void st_vk_command_list::transition(st_texture* texture_, e_st_texture_state new_state)
{
	st_vk_texture* texture = static_cast<st_vk_texture*>(texture_);
	e_st_texture_state old_state = texture->_state;

	if (old_state == new_state)
		return;

	vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor;

	if (texture->_format == st_format_d16_unorm ||
		texture->_format == st_format_d32_float)
	{
		aspect = vk::ImageAspectFlagBits::eDepth;
	}
	else if (texture->_format == st_format_d24_unorm_s8_uint)
	{
		aspect = vk::ImageAspectFlagBits::eDepth |
			vk::ImageAspectFlagBits::eStencil;
	}

	// Transition the present target to the optimal layout for copy.
	vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
		.setAspectMask(aspect)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setBaseMipLevel(0)
		.setLevelCount(texture->_levels);

	vk::ImageMemoryBarrier barriers[] =
	{
		vk::ImageMemoryBarrier()
		.setImage(texture->_handle)
		.setOldLayout(convert_resource_state(old_state))
		.setNewLayout(convert_resource_state(new_state))
		.setSubresourceRange(range),
	};

	texture->_state = new_state;

	_command_buffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eTransfer,
		vk::DependencyFlags(),
		0,
		nullptr,
		0,
		nullptr,
		1,
		barriers);
}

void st_vk_command_list::update_buffer(st_buffer* buffer_, void* data, const uint32_t offset, const uint32_t count)
{
	st_vk_buffer* buffer = static_cast<st_vk_buffer*>(buffer_);

	uint8_t* out_data;
	_device->map(buffer, 0, { 0, 0 }, (void**)&out_data);

	memcpy(out_data + offset, data, count * buffer->_element_size);

	_device->unmap(buffer, 0, { 0, 0 });
}

void st_vk_command_list::bind_resources(st_resource_table* table_)
{
	st_vk_resource_table* table = static_cast<st_vk_resource_table*>(table_);

	auto bind_set = [this](e_st_descriptor_slot slot, const vk::DescriptorSet* set, uint32_t count)
	{
		// First, copy the descriptor set to a unique one for this frame.
		vk::DescriptorSet& new_set = _descriptor_set_pool[_frame_index].emplace_back();

		vk::DescriptorSetAllocateInfo allocate_info = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(_descriptor_pool)
			.setDescriptorSetCount(1)
			.setPSetLayouts(_device->get_graphics_layout(slot));

		VK_VALIDATE(_device->get()->allocateDescriptorSets(&allocate_info, &new_set));

		vk::CopyDescriptorSet copy_set = vk::CopyDescriptorSet()
			.setDescriptorCount(count)
			.setSrcSet(*set)
			.setSrcBinding(0)
			.setDstSet(new_set)
			.setDstBinding(0);

		_device->get()->updateDescriptorSets(0, nullptr, 1, &copy_set);

		_command_buffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			*_device->get_graphics_signature(),
			slot,
			1,
			&new_set,
			0,
			nullptr);
	};

	bind_set(st_descriptor_slot_textures, &table->_textures, table->_texture_count);
	bind_set(st_descriptor_slot_samplers, &table->_samplers, table->_sampler_count);
	bind_set(st_descriptor_slot_constants, &table->_constants, table->_constant_count);
	bind_set(st_descriptor_slot_buffers, &table->_buffers, table->_buffer_count);
}

void st_vk_command_list::bind_compute_resources(st_resource_table* table_)
{
	st_vk_resource_table* table = static_cast<st_vk_resource_table*>(table_);

	auto bind_set = [this](e_st_descriptor_slot slot, const vk::DescriptorSet* set, uint32_t count)
	{
		// First, copy the descriptor set to a unique one for this frame.
		vk::DescriptorSet& new_set = _descriptor_set_pool[_frame_index].emplace_back();

		vk::DescriptorSetAllocateInfo allocate_info = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(_descriptor_pool)
			.setDescriptorSetCount(1)
			.setPSetLayouts(_device->get_compute_layout(slot));

		VK_VALIDATE(_device->get()->allocateDescriptorSets(&allocate_info, &new_set));

		vk::CopyDescriptorSet copy_set = vk::CopyDescriptorSet()
			.setDescriptorCount(count)
			.setSrcSet(*set)
			.setSrcBinding(0)
			.setDstSet(new_set)
			.setDstBinding(0);

		_device->get()->updateDescriptorSets(0, nullptr, 1, &copy_set);

		_command_buffer.bindDescriptorSets(
			vk::PipelineBindPoint::eCompute,
			*_device->get_compute_signature(),
			slot,
			1,
			&new_set,
			0,
			nullptr);
	};

	bind_set(st_descriptor_slot_textures, &table->_textures, table->_texture_count);
	bind_set(st_descriptor_slot_samplers, &table->_samplers, table->_sampler_count);
	bind_set(st_descriptor_slot_constants, &table->_constants, table->_constant_count);
	bind_set(st_descriptor_slot_uavs, &table->_uavs, table->_uav_count);
}

void st_vk_command_list::begin_render_pass(
	st_render_pass* pass_,
	st_framebuffer* framebuffer_,
	const st_clear_value* clear_values,
	const uint8_t clear_count)
{
	st_vk_render_pass* pass = static_cast<st_vk_render_pass*>(pass_);
	st_vk_framebuffer* framebuffer = static_cast<st_vk_framebuffer*>(framebuffer_);

	for (uint32_t i = 0; i < framebuffer->_targets.size(); ++i)
	{
		transition(framebuffer->_targets[i], st_texture_state_render_target);
	}
	if (framebuffer->_depth_stencil)
	{
		transition(framebuffer->_depth_stencil, st_texture_state_depth_stencil_target);
	}

	std::vector<vk::ClearValue> clears;
	clears.reserve(clear_count);
	for (uint32_t c_itr = 0; c_itr < clear_count; ++c_itr)
	{
		if (c_itr < framebuffer->_targets.size())
		{
			std::array<float, 4> c;
			std::copy(clear_values[c_itr]._color.axes, clear_values[c_itr]._color.axes + 4, c.begin());
			clears.push_back(vk::ClearValue().setColor(vk::ClearColorValue().setFloat32(c)));
		}
		else
		{
			vk::ClearDepthStencilValue ds_clear = vk::ClearDepthStencilValue()
				.setDepth(clear_values[c_itr]._depth_stencil._depth)
				.setStencil(clear_values[c_itr]._depth_stencil._stencil);
			clears.push_back(vk::ClearValue().setDepthStencil(ds_clear));
		}
	}

	vk::RenderPassBeginInfo begin_info = vk::RenderPassBeginInfo()
		.setPClearValues(clears.data())
		.setClearValueCount(clears.size())
		.setFramebuffer(framebuffer->_framebuffer)
		.setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(framebuffer->_width, framebuffer->_height)))
		.setRenderPass(pass->_render_pass);

	_command_buffer.beginRenderPass(&begin_info, vk::SubpassContents::eInline);
	_command_buffer.setViewportWithCount(1, &pass->_viewport);
}

void st_vk_command_list::end_render_pass(st_render_pass* pass, st_framebuffer* framebuffer)
{
	_command_buffer.endRenderPass();
}

#endif
