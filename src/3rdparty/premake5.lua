project "lua53"
	language "C"
	kind "StaticLib"
	
	files { "lua-5.3.3/src/*.c" }
	removefiles { "lua-5.3.3/src/luac.c" }
	
	includedirs { "lua-5.3.3/src/" }
	
project "imgui166"
	language "C++"
	kind "StaticLib"
	
	files {
		"imgui-1.66/*.cpp",
		"imgui-1.66/examples/imgui_impl_win32.cpp",
	}
	
	includedirs { "imgui-1.66" }