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
	void set_srv(const struct st_gl_texture* tex, const struct st_gl_sampler* sampler) const;
	void set_uav(const struct st_gl_texture* tex) const;
	st_gl_uniform& operator=(const st_gl_uniform& u);

	int32_t get_location() const { return _location; }
	uint32_t get_binding() const { return _binding; }

protected:
	st_gl_uniform(int32_t location, uint32_t binding);

	int32_t _location;
	uint32_t _binding;
};

/*
** Represents a shader uniform block.
** @see st_shader
*/
class st_gl_uniform_block
{
	friend class st_gl_shader;

public:
	void set(uint32_t buffer, void* data, size_t size) const;

	int32_t get_location() const { return _location; }

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
	void set(uint32_t buffer) const;

	int32_t get_location() const { return _location; }

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
	friend class st_gl_graphics_context;

public:
	st_gl_shader(const char* name, uint8_t type);
	~st_gl_shader();

	void attach(const st_gl_shader_component& shader);
	void detach(const st_gl_shader_component& shader);

	bool link();

	std::string get_link_log() const;

	const st_gl_uniform& get_uniform(uint32_t index) const;
	const st_gl_uniform_block& get_uniform_block(uint32_t index) const;
	const st_gl_shader_storage_block& get_shader_storage_block(uint32_t index) const;

	void use() const;
	
private:
	void reflect();

	uint32_t _handle;
	st_gl_shader_component* _vs;
	st_gl_shader_component* _fs;
	st_gl_shader_component* _cs;

	// Reflection information, to assist in resource table binding.
	std::vector<st_gl_uniform> _texture_binds;
	std::vector<st_gl_uniform_block> _constant_binds;
	std::vector<st_gl_shader_storage_block> _buffer_binds;
};

#endif
