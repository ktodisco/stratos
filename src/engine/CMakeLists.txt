cmake_minimum_required (VERSION 3.7)
project (stratos)

set (CMAKE_CXX_STANDARD 17)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/assimp-4.1.0/cmake-modules")

# STB: for image loading and font:
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/stb")

# GLEW: for OpenGL loading:
set (CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glew-2.0.0")
set (CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glew-2.0.0/lib/Release/x64")
find_package (GLEW REQUIRED)
include_directories (${GLEW_INCLUDE_DIRS})
link_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/glew-2.0.0/lib/Release/x64")

# LUA: for scripting:
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/lua-5.3.3/src")
file (GLOB_RECURSE LUA_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/lua-5.3.3/src/*.c)
list (REMOVE_ITEM LUA_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/lua-5.3.3/src/luac.c)
add_library (lua53 ${LUA_SOURCE_FILES})

# PIX 3:
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/WinPixEventRuntime.1.0.170918004/Include/WinPixEventRuntime")
link_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/WinPixEventRuntime.1.0.170918004/bin")

# ImGui: for tooling:
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/imgui-1.66")
file (GLOB IMGUI_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/imgui-1.66/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/imgui-1.66/examples/imgui_impl_dx12.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/imgui-1.66/examples/imgui_impl_opengl3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/imgui-1.66/examples/imgui_impl_win32.cpp)
add_library (imgui166 ${IMGUI_SOURCE_FILES})
target_link_libraries(imgui166 glew32 opengl32 glu32 d3d12 D3DCompiler)
target_compile_definitions (imgui166 PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW)

# Assimp: for asset importing:
find_package (assimp REQUIRED)
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/assimp-4.1.0/include")
link_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/assimp-4.1.0/lib/Release")

# Vulkan headers:
# TODO: This pre-installed path is not ideal.
# Try avoiding the library link by calling vk::init.
include_directories("C:/VulkanSDK/1.3.204.1/Include")
link_directories ("C:/VulkanSDK/1.3.204.1/Lib")

# ST framework:
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")
file (GLOB_RECURSE ST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# On Windows, we're not going to worry about CRT secure warnings.
if (MSVC)
	set (CMAKE_CXX_FLAGS "$(CMAKE_CXX_FLAGS) /EHsc /MP")
	set (CMAKE_C_FLAGS "$(CMAKE_C_FLAGS) /MP")
endif()

# For Unix, tell gcc to use c++11.
if (MINGW)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_POSIX_C_SOURCE")
endif()

add_executable (stratos ${ST_SOURCE_FILES} always_copy_data.h)
target_link_libraries (stratos glew32s opengl32 glu32 lua53 imgui166 d3d12 dxgi dxguid D3DCompiler WinPixEventRuntime assimp-vc140-mt vulkan-1.lib)
if (MSVC)
	set_target_properties (stratos PROPERTIES LINK_FLAGS "/ignore:4098 /ignore:4099")
endif()

add_custom_command (TARGET stratos PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/ttf-bitstream-vera-1.10/VeraMono.ttf $<TARGET_FILE_DIR:stratos>)
add_custom_command (TARGET stratos PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/WinPixEventRuntime.1.0.170918004/bin/WinPixEventRuntime.dll $<TARGET_FILE_DIR:stratos>)
add_custom_command (TARGET stratos PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/assimp-4.1.0/bin/Release/assimp-vc140-mt.dll $<TARGET_FILE_DIR:stratos>)

add_custom_target (ALWAYS_COPY_DATA COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/always_copy_data.h)
add_custom_command (TARGET ALWAYS_COPY_DATA POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../data $<TARGET_FILE_DIR:stratos>/data)

add_custom_command (TARGET ALWAYS_COPY_DATA PRE_BUILD COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/../../build/scripts/build_shaders.py ${CMAKE_CURRENT_SOURCE_DIR}/../../data/shaders glsl $<TARGET_FILE_DIR:stratos>/data/shaders)
add_custom_command (TARGET ALWAYS_COPY_DATA PRE_BUILD COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/../../build/scripts/build_shaders.py ${CMAKE_CURRENT_SOURCE_DIR}/../../data/shaders spirv $<TARGET_FILE_DIR:stratos>/data/shaders)
add_custom_command (TARGET ALWAYS_COPY_DATA PRE_BUILD COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/../../build/scripts/build_shaders.py ${CMAKE_CURRENT_SOURCE_DIR}/../../data/shaders dxil $<TARGET_FILE_DIR:stratos>/data/shaders)
