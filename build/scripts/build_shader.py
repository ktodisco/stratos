import os
import subprocess
import sys

def convert_hlsl_to_glsl(hlsl_file, type, output_dir):
	converter = os.path.join("..", "ShaderConductor", "Bin", "Release", "ShaderConductorCmd.exe")
	
	basename = os.path.splitext(os.path.basename(hlsl_file))[0]
	
	file_params = []
	if type == 'vps':
		file_params.append([basename + "_vert.glsl", "vs_main", "vs"])
		file_params.append([basename + "_frag.glsl", "ps_main", "ps"])
	elif type == 'cs':
		file_params.append([basename + "_comp.glsl", "cs_main", "cs"])
	
	print("Converting %s to %s." % (hlsl_file, " and ".join([p[0] for p in file_params])))
	
	# Run conversions.
	for params in file_params:
		subprocess.call([converter, "--input", hlsl_file, "--output", os.path.join(output_dir, params[0]), "--entry", params[1], "--stage", params[2], "--target", "glsl"])
		
def convert_hlsl_to_spirv(hlsl_file, type, output_dir):
	converter = os.path.join("..", "ShaderConductor", "Bin", "Release", "ShaderConductorCmd.exe")
	
	basename = os.path.splitext(os.path.basename(hlsl_file))[0]
	
	file_params = []
	if type == 'vps':
		file_params.append([basename + "_vert.spirv", "vs_main", "vs"])
		file_params.append([basename + "_frag.spirv", "ps_main", "ps"])
	elif type == 'cs':
		file_params.append([basename + "_comp.spirv", "cs_main", "cs"])
	
	print("Converting %s to %s." % (hlsl_file, " and ".join([p[0] for p in file_params])))
	
	# Run conversions.
	for params in file_params:
		subprocess.call([converter, "--input", hlsl_file, "--output", os.path.join(output_dir, params[0]), "--entry", params[1], "--stage", params[2], "--target", "spirv"])

def build_sm6(hlsl_file, type, output_dir):
	converter = os.path.join("..", "DirectXShaderCompiler", "dxc.exe")
	
	basename = os.path.splitext(os.path.basename(hlsl_file))[0]
	
	file_params = []
	if type == 'vps':
		file_params.append([basename + "_vert.cso", "vs_main", "vs_6_0"])
		file_params.append([basename + "_frag.cso", "ps_main", "ps_6_0"])
	elif type == 'cs':
		file_params.append([basename + "_comp.cso", "cs_main", "cs_6_0"])
	vertex_file = basename + "_vert.cso"
	fragment_file = basename + "_frag.cso"
	
	print("Converting %s to %s." % (hlsl_file, " and ".join([p[0] for p in file_params])))
	
	# Run conversions.
	for params in file_params:
		subprocess.call([converter, "-E", params[1], "-T", params[2], "-Fo", os.path.join(output_dir, params[0]), hlsl_file])

if __name__ == "__main__":
	usage = [__file__, "<input file>", "<target> (glsl|spirv|dxil)", "<type> (vps|cs)", "<output director>"]
	
	if not len(sys.argv) == len(usage):
		print("Usage: " + ' '.join(usage))
		sys.exit(1)
		
	input_file = os.path.normpath(sys.argv[1])
	target_type = sys.argv[2]
	shader_type = sys.argv[3]
	output_dir = os.path.normpath(sys.argv[4])
	
	# Ensure this directory exists.
	if not os.path.exists(output_dir):
		os.makedirs(output_dir)
		
	if target_type == 'glsl':
		convert_hlsl_to_glsl(input_file, shader_type, output_dir)
	if target_type == 'spirv':
		convert_hlsl_to_spirv(input_file, shader_type, output_dir)
	if target_type == 'dxil':
		build_sm6(input_file, shader_type, output_dir)
