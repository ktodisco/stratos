#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_program.h"
#include "st_texture.h"

#include "math/st_mat4f.h"
#include "math/st_vec3f.h"

#include <string>

/*
** Base class for all graphical materials.
** Includes the shaders and other state necessary to draw geometry.
*/
class st_material
{
public:
	virtual bool init() = 0;

	virtual void bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform) = 0;

	virtual void set_color(const st_vec3f& color) {}

protected:
	bool init_shaders(const char* vs, const char* fs);

protected:
	st_shader* _vs;
	st_shader* _fs;
	st_program* _program;
};

/*
** Simple unlit, single textured material.
*/
class st_unlit_texture_material : public st_material
{
public:
	st_unlit_texture_material(const char* texture_file);
	~st_unlit_texture_material();

	virtual bool init() override;

	virtual void bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform) override;

private:
	std::string _texture_file;
	st_texture* _texture;
};

/*
** Simple untextured material with a constant color.
*/
class st_constant_color_material : public st_material
{
public:
	st_constant_color_material();
	~st_constant_color_material();

	virtual bool init() override;

	virtual void bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform) override;

	virtual void set_color(const st_vec3f& color) override { _color = color; }

private:
	st_vec3f _color;
};

/*
** A material supporting the phong shading model.
*/
class st_phong_material : public st_material
{
public:
	st_phong_material();
	~st_phong_material();

	virtual bool init() override;
	virtual void bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform) override;
};

/*
** A material which supports vertex animation.
*/
class st_animated_material : public st_material
{
public:
	st_animated_material(struct st_skeleton* skeleton);
	~st_animated_material();

	virtual bool init() override;
	virtual void bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform) override;

private:
	struct st_skeleton* _skeleton;
};

/*
** A material which supports vertex animation and a texture.
*/
class st_animated_unlit_texture_material : public st_material
{
public:
	st_animated_unlit_texture_material(struct st_skeleton* skeleton, const char* texture_file);
	~st_animated_unlit_texture_material();

	virtual bool init() override;
	virtual void bind(const st_mat4f& proj, const st_mat4f& view, const st_mat4f& transform) override;

private:
	std::string _texture_file;
	st_texture* _texture;

	struct st_skeleton* _skeleton;
};
