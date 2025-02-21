project "stratos"
	language "C++"
	cppdialect "C++17"
	kind "ConsoleApp"
	warnings "Off"

	flags { "MultiProcessorCompile" }
	
	defines {
		"_CRT_SECURE_NO_WARNINGS",
	}
	
	files {
		"**.h",
		"**.cpp",
	}
	
	includedirs {
		"../3rdparty/glew-2.0.0/include",
		"../3rdparty/stb",
		"../3rdparty/lua-5.3.3/src",
		"../3rdparty/WinPixEventRuntime.1.0.170918004/Include/WinPixEventRuntime",
		"../3rdparty/imgui-1.66",
		"../3rdparty/assimp-5.4.3/include",
		"../3rdparty/vulkan/1.3.290.0/include",
		
		"./",
	}
	
	libdirs {
		"../3rdparty/glew-2.0.0/lib/Release/x64",
		"../3rdparty/WinPixEventRuntime.1.0.170918004/bin",
		"../3rdparty/vulkan/1.3.290.0/Lib",
	}
	
	links {
		"glew32s",
		"opengl32",
		"glu32",
		"lua53",
		"imgui166",
		"d3d12",
		"dxgi",
		"dxguid",
		"D3DCompiler",
		"WinPixEventRuntime",
		"assimp-5.4.3",
		"vulkan-1",
	}
	
	postbuildcommands {
		"{COPYFILE} %[../3rdparty/ttf-bitstream-vera-1.10/VeraMono.ttf] %[%{!cfg.targetdir}]",
		"{COPYFILE} %[../3rdparty/WinPixEventRuntime.1.0.170918004/bin/WinPixEventRuntime.dll] %[%{!cfg.targetdir}]",
	}
	
	buildoptions {
		"/W2 /WX",
	}
	
	linkoptions {
		"/IGNORE:4099",
	}