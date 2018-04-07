import os
import subprocess
import sys

def convert_hlsl_to_glsl(shader_dir, hlsl_files, output_dir):
	converter = "../../src/3rdparty/HLSL2GLSLConverter/win64/HLSL2GLSLConverter.exe"
	
	for hlsl_file in hlsl_files:
		basename = os.path.splitext(hlsl_file)[0]
		vertex_file = basename + "_vert.glsl"
		fragment_file = basename + "_frag.glsl"
		
		print("Converting %s to %s and %s." % (hlsl_file, vertex_file, fragment_file))
		
		# Run conversions.
		subprocess.call([converter, "-d", shader_dir, "-i", hlsl_file, "-e", "vs_main", "-t", "vs", "-o", os.path.join(output_dir, vertex_file)])
		subprocess.call([converter, "-d", shader_dir, "-i", hlsl_file, "-e", "ps_main", "-t", "ps", "-o", os.path.join(output_dir, fragment_file)])

if __name__ == "__main__":
	usage = [__file__, "<directory>", "<target> (glsl)", "<output directory>"]
	
	if not len(sys.argv) == len(usage):
		print("Usage: " + ' '.join(usage))
		sys.exit(1)
		
	shader_directory = sys.argv[1]
	target_type = sys.argv[2]
	output_directory = sys.argv[3]
	
	for (dirpath, dirnames, filenames) in os.walk(shader_directory):
		# Collect all .hlsl files.
		hlsl_files = [file for file in filenames if file.endswith(".hlsl")]
		
		if target_type == 'glsl':
			convert_hlsl_to_glsl(shader_directory, hlsl_files, output_directory)
