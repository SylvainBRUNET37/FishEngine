#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <assimp/mesh.h>

#include "shapes/Model.h"

class ModelLoader
{
public:
	[[nodiscard]] Model LoadModel(const std::string& filename);

private:
	using MeshData = aiMesh*;

	MeshData meshData = nullptr;

	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	void LoadVertices();
	void LoadIndices();
	void LoadMeshData(const std::string& filename);
};

#endif
