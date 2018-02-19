#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include "math/st_mat4f.h"
#include "math/st_vec3f.h"

#include <string>

#include <Windows.h>
#include <wrl.h>

/*
** Base class for all graphical materials.
** Includes the shaders and other state necessary to draw geometry.
*/
class st_dx12_material
{
public:
	virtual void bind(
		class st_render_context* context,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) = 0;

	virtual void get_pipeline_state(
		struct st_dx12_pipeline_state_desc* state_desc) = 0;

protected:
	// TODO: Instead of a program and uniforms, the material interface needs
	// constants, or params, which are set during bind.
	// The params authority can be an object shared to the material by the render pass.
	// In DX12, it represents the CBV descriptors.
	// In OpenGL, it's an intermediary construct which eventually performs or directs glUniform calls.

	// pipeline state needs to be generic enough to account for outside configuration.
	// material owns the shader and pipeline state

	// probably need an st_constant_buffer class which manages parameters by name

	// pipeline state is constructed out of a shader and other state settings
	// let's say a pipeline state descriptor object with generic enum values

	// creating a material would look something like this:
	// //calling base init_shaders with shader name
	// creating one or more st_constant_buffers and calling add_constant("name", type) to them
	// adding each constant buffer to the material via add_constant_buffer
	// //creating a pipeline state object with state descriptor object

	// who owns the constant buffer data upload heap? the constant buffer object
	// constant buffer object also owns the CBV
	// OpenGL implementation gives the program pointer to the CB object in add_constant_buffer
	// this still leaves a big open question about frame-universal CBs and how they'd work for OpenGL
	// maybe add_constant("name", type, number)?

	// on material bind:
	// //pipeline state is made active on the context - state->activate(context);
	// note that this would allow the OpenGL implementation to do something like context->set_constants(this (state) ->_program) (for universal CBs)
	// constant buffer data is updated - xxx_cb->set("name", value);
	// descriptor tables are updated - context->set_constant_buffers(offset_to_first) (no-op on OpenGL?)

	// NEW IDEA
	// render pass contains default materials
	// material has get_pipeline_state which returns required blend/rasterizer/depth state
	// default materials are used to create PSOs for the render pass
	// later on, materials can have keys in order to match materials for rendering
	// render pass creates default materials then uses them to create PSOs by combining with render target configs
	// models also have instances of materials which override the default material parameters

	// New problem - should shader and constant buffer storage for materials be static?
	// Why recreate shaders and constant buffers for every object using the same materials/shaders?
	// Create all shader singletons at startup.
	// Pass st_shader_type::get_instance() to materials on material creation.
	// Constant buffers and their upload heaps cannot be shared because data is not immediately read, it is recorded.
	// Therefore, materials have constant buffer objects as explained above
};
