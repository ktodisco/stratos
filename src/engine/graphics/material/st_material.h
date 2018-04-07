#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/st_texture.h>

#include <math/st_mat4f.h>
#include <math/st_vec3f.h>

#include <memory>
#include <string>

#include <wrl.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_material.h>

typedef st_gl_material st_platform_material;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_material.h>

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
	st_material_type_deferred_light,
	st_material_type_tonemap,
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
