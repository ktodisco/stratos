#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <cstdint>
#include <string>

/*
** Represents a shader uniform (constant).
** @see st_gl_shader
*/
class st_gl_uniform
{
	friend class st_gl_shader;

public:
	void set(float val);
	void set(const struct st_vec2f& vec);
	void set(const struct st_vec3f& vec);
	// MSVC linker has trouble comprehending the validity of the const definition,
	// so work around it with a non-const definition.
	void set(struct st_vec3f& vec);
	void set(const struct st_vec4f& vec);
	void set(const struct st_mat4f& mat);
	void set(const struct st_mat4f* mats, uint32_t count);
	void set(const struct st_gl_texture& tex, uint32_t unit);

	int32_t get_location() const { return _location; }

protected:
	st_gl_uniform(int32_t location);

	const int32_t _location;
};

/*
** Represents a shader uniform block.
** @see st_shader
*/
class st_gl_uniform_block
{
	friend class st_gl_shader;

public:
	void set(uint32_t buffer, void* data, size_t size);

protected:
	st_gl_uniform_block(int32_t location);

	const int32_t _location;
};

/*
** Represents a shader storage buffer resource.  Equivalent of structured buffer.
** @see st_shader
*/
class st_gl_shader_storage_block
{
	friend class st_gl_shader;

public:
	void set(uint32_t buffer, void* data, size_t size);

protected:
	st_gl_shader_storage_block(int32_t location);

	const int32_t _location;
};

/*
** Represents a shader.
*/
class st_gl_shader_component
{
	friend class st_gl_shader;

public:
	st_gl_shader_component(const char* source, uint32_t type);
	~st_gl_shader_component();

	bool compile();

	std::string get_compile_log() const;

private:
	uint32_t _handle;
	const char* _source;
};

/*
** Represents a full shading pipeline.
** Vertex shader and fragment shader linked together.
** @see st_shader
*/
class st_gl_shader : public st_shader
{
public:
	st_gl_shader(const char* source, uint8_t type);
	~st_gl_shader();

	void attach(const st_gl_shader_component& shader);
	void detach(const st_gl_shader_component& shader);

	bool link();

	std::string get_link_log() const;

	st_gl_uniform get_uniform(const char* name) const;
	st_gl_uniform_block get_uniform_block(const char* name) const;
	st_gl_shader_storage_block get_shader_storage_block(const char* name) const;

	void use() const;
	
	uint32_t _handle;
	st_gl_shader_component* _vs;
	st_gl_shader_component* _fs;
};

#endif
