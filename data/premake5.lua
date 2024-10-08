project "data"
	kind "Utility"
	
	files {
		"animations/*.*",
		"models/*.*",
		"scripts/*.*",
		"shaders/*.hlsl",
		"textures/*.*",
		"xnormal_test/*.*",
	}
	
	filter { "files:shaders/*.hlsl" }
		buildmessage "Building shader file %{Filename}"
		buildcommands {
			"python $(ProjectDir)../scripts/build_shader.py %(FullPath) spirv $(OutDir)data/shaders",
			"python $(ProjectDir)../scripts/build_shader.py %(FullPath) dxil $(OutDir)data/shaders",
			"python $(ProjectDir)../scripts/build_shader.py %(FullPath) glsl $(OutDir)data/shaders",
		}
		buildoutputs {
			"$(OutDir)data/shaders/%(Filename)_vert.spirv", "$(OutDir)data/shaders/%(Filename)_frag.spirv",
			"$(OutDir)data/shaders/%(Filename)_vert.cso", "$(OutDir)data/shaders/%(Filename)_frag.cso",
			"$(OutDir)data/shaders/%(Filename)_vert.glsl", "$(OutDir)data/shaders/%(Filename)_frag.glsl",
		}
		
	filter {}
	
	prebuildcommands {
		"{COPYDIR} %[./] %[%{!cfg.targetdir}/data]",
	}