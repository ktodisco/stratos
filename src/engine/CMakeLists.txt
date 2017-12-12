cmake_minimum_required (VERSION 3.7)
project (stratos)

# STB: for image loading and font:
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/stb")

# GLEW: for OpenGL loading:
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glew-2.0.0")
set(CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glew-2.0.0/lib/Release/x64")
find_package(GLEW REQUIRED)
include_directories (${GLEW_INCLUDE_DIRS})
link_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glew-2.0.0/lib/Release/x64")

# LUA: for scripting:
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/lua-5.3.3/src")
file(GLOB_RECURSE LUA_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/lua-5.3.3/src/*.c)
list(REMOVE_ITEM LUA_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/lua-5.3.3/src/luac.c)
add_library(lua53 ${LUA_SOURCE_FILES})

# ST framework:
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE ST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# On Windows, we're not going to worry about CRT secure warnings.
if (MSVC)
	set(CMAKE_CXX_FLAGS "$(CMAKE_CXX_FLAGS) /EHsc")
endif()

# For Unix, tell gcc to use c++11.
if (MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_POSIX_C_SOURCE")
endif()

add_executable(stratos ${ST_SOURCE_FILES} always_copy_data.h)
target_link_libraries (stratos glew32s opengl32 lua53)
if (MSVC)
	set_target_properties(stratos PROPERTIES LINK_FLAGS "/ignore:4098 /ignore:4099")
endif()

add_custom_command(TARGET stratos PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/ttf-bitstream-vera-1.10/VeraMono.ttf $<TARGET_FILE_DIR:stratos>)

add_custom_target(ALWAYS_COPY_DATA COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/always_copy_data.h)
add_dependencies(stratos ALWAYS_COPY_DATA)

add_custom_command(TARGET stratos POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../data $<TARGET_FILE_DIR:stratos>/data)
