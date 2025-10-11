#include "pch.h"
#include "rendering/ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "rendering/utils/VerboseAssertion.h"
#include "rendering/shapes/Mesh.h"

using namespace std;
using namespace DirectX;

Model ModelLoader::LoadModel(const std::string& filename)
{
	LoadMeshData(filename);
	LoadVertices();
	LoadIndices();

	//return Mesh{std::move(vertices), std::move(indices)};

	return Model{}; // TODO
}

void ModelLoader::LoadMeshData(const std::string& filename)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		filename,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices
	);

	vassert(scene && scene->mRootNode, "Could not load mesh: " + filename);

	// Load only the first mesh
	meshData = scene->mMeshes[0];
}

void ModelLoader::LoadVertices()
{
	vertices.reserve(meshData->mNumVertices);

	for (unsigned int i = 0; i < meshData->mNumVertices; i++)
	{
		XMFLOAT3 position(meshData->mVertices[i].x, meshData->mVertices[i].y, meshData->mVertices[i].z);

		XMFLOAT3 normal(0, 0, 0);
		if (meshData->HasNormals())
			normal = XMFLOAT3(meshData->mNormals[i].x, meshData->mNormals[i].y, meshData->mNormals[i].z);

		XMFLOAT2 texCoord(0, 0);
		if (meshData->HasTextureCoords(0))
			texCoord = XMFLOAT2(meshData->mTextureCoords[0][i].x, meshData->mTextureCoords[0][i].y);

		vertices.emplace_back(position, normal, texCoord);
	}
}

void ModelLoader::LoadIndices()
{
	constexpr unsigned int TRIANGLE_SIDE_NUMBER = 3;
	indices.reserve(meshData->mNumFaces * TRIANGLE_SIDE_NUMBER);

	for (unsigned int i = 0; i < meshData->mNumFaces; i++)
	{
		const aiFace& face = meshData->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
}
