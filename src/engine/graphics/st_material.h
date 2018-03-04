#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include "st_texture.h"

#include "math/st_mat4f.h"
#include "math/st_vec3f.h"

#include <memory>
#include <string>

#include <wrl.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/opengl/st_gl_material.h>

typedef st_gl_material st_platform_material;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/dx12/st_dx12_material.h>

typedef st_dx12_material st_platform_material;
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

// For the record, I'm not fond of this direction.
enum st_material_type
{
	st_material_type_unlit_texture,
	st_material_type_constant_color,
	st_material_type_phong,
	st_material_type_font,
	st_material_type_gbuffer,
	st_material_type_fullscreen,
};

/*
** Base class for all graphical materials.
** Includes the shaders and other state necessary to draw geometry.
*/
class st_material : public st_platform_material
{
public:
	virtual void set_color(const st_vec3f& color) {}
	virtual st_material_type get_material_type() = 0;
};

struct st_view_cb
{
	st_mat4f _mvp;
};

// TODO: These should be moved into their own files.
/*
** Simple unlit, single textured material.
*/
class st_unlit_texture_material : public st_material
{
public:
	st_unlit_texture_material(const char* texture_file);
	~st_unlit_texture_material();

	virtual void bind(
		class st_render_context* context,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_unlit_texture; }

private:
	// TODO: This view buffer should live at a larger scope.
	std::unique_ptr<class st_constant_buffer> _view_buffer = nullptr;
	std::string _texture_file;
	std::unique_ptr<st_texture> _texture;
};

/*
** Simple untextured material with a constant color.
*/
class st_constant_color_material : public st_material
{
public:
	st_constant_color_material();
	~st_constant_color_material();

	//virtual bool init() override;

	virtual void bind(
		class st_render_context* context,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_constant_color; }

	void set_color(const st_vec3f& color) override { _color = color; }

	struct st_constant_color_cb
	{
		st_mat4f _mvp;
		st_vec3f _color;
	};

private:
	std::unique_ptr<class st_constant_buffer> _color_buffer = nullptr;
	st_vec3f _color;
};

/*
** A material supporting the phong shading model.
*/
class st_phong_material final : public st_material
{
public:
	st_phong_material();
	~st_phong_material();

	void bind(
		class st_render_context* context,
		const st_mat4f& proj,
		const st_mat4f& view,
		const st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_phong; }

private:
	std::unique_ptr<class st_constant_buffer> _phong_buffer = nullptr;
};
