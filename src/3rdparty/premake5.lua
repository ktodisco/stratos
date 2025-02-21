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
	
project "assimp-5.4.3"
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"

	flags { "MultiProcessorCompile" }
	
	defines {
		"WIN32_LEAN_AND_MEAN",
		
		"ASSIMP_BUILD_NO_C4D_IMPORTER",
		"ASSIMP_BUILD_NO_USD_IMPORTER",
		"ASSIMP_BUILD_NO_IFC_IMPORTER",
		"ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
		"ASSIMP_BUILD_NO_VRML_IMPORTER",
		"ASSIMP_BUILD_NO_GLTF_IMPORTER",
		"ASSIMP_BUILD_NO_X3D_IMPORTER"
	}
	
	files {
		"assimp-5.4.3/contrib/clipper/*.hpp",
		"assimp-5.4.3/contrib/clipper/*.cpp",
		"assimp-5.4.3/contrib/pugixml/*.hpp",
		"assimp-5.4.3/contrib/pugixml/*.cpp",
		"assimp-5.4.3/contrib/rapidjson/**.h",
		"assimp-5.4.3/contrib/stb/*.h",
		"assimp-5.4.3/contrib/unzip/*.h",
		"assimp-5.4.3/contrib/unzip/*.c",
		"assimp-5.4.3/contrib/utf8cpp/*.h",
		"assimp-5.4.3/contrib/zlib/*.h",
		"assimp-5.4.3/contrib/zlib/*.c",
		
		"assimp-5.4.3/include/assimp/Compiler/*.h",
		"assimp-5.4.3/include/assimp/*.h",
		"assimp-5.4.3/include/assimp/*.hpp",
		"assimp-5.4.3/include/assimp/*.inl",
		"assimp-5.4.3/include/assimp/config.h.in",
		
		"assimp-5.4.3/code/Common/*.cpp",
		"assimp-5.4.3/code/Common/*.h",
		"assimp-5.4.3/code/CApi/*.h",
		"assimp-5.4.3/code/CApi/*.cpp",
		"assimp-5.4.3/code/Geometry/*.h",
		"assimp-5.4.3/code/Geometry/*.cpp",
		"assimp-5.4.3/code/Material/*.h",
		"assimp-5.4.3/code/Material/*.cpp",
		"assimp-5.4.3/code/Pbrt/*.h",
		"assimp-5.4.3/code/Pbrt/*.cpp",
		"assimp-5.4.3/code/PostProcessing/*.h",
		"assimp-5.4.3/code/PostProcessing/*.cpp",
		
		"assimp-5.4.3/code/AssetLib/**.h",
		"assimp-5.4.3/code/AssetLib/**.cpp",
	}
	
	removefiles {
		"assimp-5.4.3/contrib/draco/*.h",
		
		"assimp-5.4.3/code/AssetLib/C4D/*.h",
		"assimp-5.4.3/code/AssetLib/C4D/*.cpp",
		"assimp-5.4.3/code/AssetLib/USD/*.h",
		"assimp-5.4.3/code/AssetLib/USD/*.cpp",
		"assimp-5.4.3/code/AssetLib/IFC/*.h",
		"assimp-5.4.3/code/AssetLib/IFC/*.cpp",
		"assimp-5.4.3/code/AssetLib/OpenGEX/*.h",
		"assimp-5.4.3/code/AssetLib/OpenGEX/*.cpp",
		"assimp-5.4.3/code/AssetLib/VRML/*.h",
		"assimp-5.4.3/code/AssetLib/VRML/*.cpp",
		"assimp-5.4.3/code/AssetLib/glTF/*.h",
		"assimp-5.4.3/code/AssetLib/glTF/*.cpp",
		"assimp-5.4.3/code/AssetLib/glTF2/*.h",
		"assimp-5.4.3/code/AssetLib/glTF2/*.cpp",
		"assimp-5.4.3/code/AssetLib/X3D/*.h",
		"assimp-5.4.3/code/AssetLib/X3D/*.cpp",
	}
	
	includedirs {
		"assimp-5.4.3/contrib",
		"assimp-5.4.3/contrib/pugixml/src",
		"assimp-5.4.3/contrib/rapidjson/include",
		"assimp-5.4.3/contrib/unzip",
		"assimp-5.4.3/contrib/utf8cpp/source",
		"assimp-5.4.3/contrib/zlib",
		"assimp-5.4.3/include",
		"assimp-5.4.3/",
		"assimp-5.4.3/code",
	}
	
	disablewarnings { "4996" }
