/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_lua_component.h>

#include <entity/st_entity.h>

#include <framework/st_frame_params.h>

#include <lua.hpp>

#include <cassert>
#include <iostream>
#include <string>

st_lua_component::st_lua_component(st_entity* ent, const char* path) : st_component(ent)
{
	_lua = luaL_newstate();
	luaL_openlibs(_lua);

	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	int status = luaL_loadfile(_lua, fullpath.c_str());
	if (status)
	{
		std::cerr << "Failed to load script " << path << ": " << lua_tostring(_lua, -1);
		lua_close(_lua);
		_lua = nullptr;
		return;
	}

	lua_pcall(_lua, 0, 0, 0);
	lua_getglobal(_lua, "update");
	int top = lua_gettop(_lua);
	if (top == 0 || lua_isfunction(_lua, top) == 0)
	{
		std::cerr << "Script " << path << " does not contain 'update' function." << std::endl;
		lua_close(_lua);
		_lua = nullptr;
		return;
	}

	lua_pop(_lua, 1);

	lua_register(_lua, "frame_params_get_input_left", lua_frame_params_get_input_left);
	lua_register(_lua, "frame_params_get_input_right", lua_frame_params_get_input_right);
	lua_register(_lua, "component_get_entity", lua_component_get_entity);
	lua_register(_lua, "entity_translate", lua_entity_translate);
}

st_lua_component::~st_lua_component()
{
	if (_lua)
	{
		lua_close(_lua);
	}
}

void st_lua_component::update(st_frame_params* params)
{
	if (_lua)
	{
		lua_getglobal(_lua, "update");
		lua_pushlightuserdata(_lua, this);
		lua_pushlightuserdata(_lua, params);
		int status = lua_pcall(_lua, 2, 0, 0);
		if (status)
		{
			std::cerr << "Error: " << lua_tostring(_lua, -1);
			assert(0);
		}
	}
}

int st_lua_component::lua_frame_params_get_input_left(lua_State* state)
{
	int arg_count = lua_gettop(state);
	if (arg_count == 1)
	{
		st_frame_params* params = (st_frame_params*)lua_touserdata(state, 1);
		lua_pushboolean(state, (params->_button_mask & k_button_left) != 0);
	}
	else
	{
		std::cerr << "Function frame_params_get_input_left expected 1 argument but got " << arg_count << "." << std::endl;
	}
	return 1;
}

int st_lua_component::lua_frame_params_get_input_right(lua_State* state)
{
	int arg_count = lua_gettop(state);
	if (arg_count == 1)
	{
		st_frame_params* params = (st_frame_params*)lua_touserdata(state, 1);
		lua_pushboolean(state, (params->_button_mask & k_button_right) != 0);
	}
	else
	{
		std::cerr << "Function frame_params_get_input_right expected 1 argument but got " << arg_count << "." << std::endl;
	}
	return 1;
}

int st_lua_component::lua_component_get_entity(lua_State* state)
{
	int arg_count = lua_gettop(state);
	if (arg_count == 1)
	{
		st_lua_component* component = (st_lua_component*)lua_touserdata(state, 1);
		lua_pushlightuserdata(state, component->get_entity());
	}
	else
	{
		std::cerr << "Function component_get_entity expected 1 argument but got " << arg_count << "." << std::endl;
	}
	return 1;
}

int st_lua_component::lua_entity_translate(lua_State* state)
{
	int arg_count = lua_gettop(state);
	if (arg_count == 4)
	{
		st_entity* ent = (st_entity*)lua_touserdata(state, 1);

		st_vec3f vec;
		vec.x = (float)lua_tonumber(state, 2);
		vec.y = (float)lua_tonumber(state, 3);
		vec.z = (float)lua_tonumber(state, 4);

		ent->translate(vec);
	}
	else
	{
		std::cerr << "Function entity_translate expected 4 arguments but got " << arg_count << "." << std::endl;
	}
	return 0;
}
