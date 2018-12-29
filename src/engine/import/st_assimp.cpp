/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <import/st_assimp.h>

#include <graphics/geometry/st_model_data.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cassert>

void assimp_import_model(const char* filename, st_model_data* model)
{
	// Create an instance of the importer class.
	Assimp::Importer importer;

	// Have it read the given file with some postprocessing.
	const aiScene* scene = importer.ReadFile(
		"C:\\Projects\\stratos\\data\\models\\sphere.ply",
		aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if (!scene)
	{
		printf("An error was encountered during the import:\n\t%s\n", importer.GetErrorString());
		assert(false);
	}

	// Extract the assimp data into our runtime model format.
	if (scene->HasMeshes())
	{
		aiMesh* mesh = scene->mMeshes[0];

		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		{
			st_vertex vertex;
			aiVector3D* position = (mesh->mVertices + i);
			aiVector3D* normal = (mesh->mNormals + i);
			aiVector3D* tangent = (mesh->mTangents + i);
			aiVector3D* uv = (mesh->mTextureCoords[0] + i);

			vertex._position = { position->x, position->y, position->z };
			vertex._normal = { normal->x, normal->y, normal->z };
			vertex._tangent = { tangent->x, tangent->y, tangent->z };
			vertex._uv = { uv->x, uv->y };

			model->_vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace* face = (mesh->mFaces + i);

			model->_indices.push_back(face->mIndices[0]);
			model->_indices.push_back(face->mIndices[1]);
			model->_indices.push_back(face->mIndices[2]);

			if (face->mNumIndices > 3)
			{
				model->_indices.push_back(face->mIndices[0]);
				model->_indices.push_back(face->mIndices[2]);
				model->_indices.push_back(face->mIndices[3]);
			}
		}
	}

	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_normal, 1));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_tangent, 2));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_color, 3));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_uv, 4));

	model->_vertex_format.finalize();
}
