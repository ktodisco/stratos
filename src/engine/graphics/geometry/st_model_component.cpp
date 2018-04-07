/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/geometry/st_model_component.h>

#include <graphics/animation/st_animation.h>
#include <graphics/geometry/st_geometry.h>
#include <graphics/geometry/st_model_data.h>
#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/geometry/st_vertex_format.h>
#include <graphics/material/st_material.h>

#include <entity/st_entity.h>

#include <cassert>

st_model_component::st_model_component(st_entity* ent, st_model_data* model, st_material* material) :
	st_component(ent),
	_material(material)
{
	assert(model->_vertex_format.is_finalized());

	_geometry = std::make_unique<st_geometry>(
		&model->_vertex_format,
		&model->_vertices[0],
		(uint32_t)sizeof(model->_vertices[0]),
		(uint32_t)model->_vertices.size(),
		&model->_indices[0],
		(uint32_t)model->_indices.size());
}

st_model_component::~st_model_component()
{
}

void st_model_component::update(st_frame_params* params)
{
	st_static_drawcall draw_call;
	draw_call._name = "st_model_component";
	draw_call._transform = get_entity()->get_transform();
	draw_call._material = _material;
	_geometry->draw(draw_call);

	while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_static_drawcalls.push_back(draw_call);
	params->_static_drawcall_lock.clear(std::memory_order_release);
}
