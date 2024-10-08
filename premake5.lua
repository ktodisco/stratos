workspace "stratos"
	configurations { "Debug", "Development", "Profile", "Release" }
	platforms { "x64" }
	location "build/st-win64"
	characterset "ASCII"
	
	targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
	
	filter { "configurations:Debug" }
		defines { "_DEBUG" }
		optimize "Off"
		symbols "On"
		
	filter { "configurations:Development" }
		defines { "_DEVELOPMENT" }
		optimize "Debug"
		symbols "On"
		
	filter { "configurations:Profile" }
		defines { "_PROFILE" }
		optimize "Speed"
		symbols "Off"
		
	filter { "configurations:Release" }
		defines { "_RELEASE" }
		optimize "Full"
		symbols "Off"
		
	filter { "platforms:x64" }
		architecture "x86_64"
		
	filter {}
	
include("src/3rdparty/premake5.lua")
include("src/engine/premake5.lua")
include("data/premake5.lua")