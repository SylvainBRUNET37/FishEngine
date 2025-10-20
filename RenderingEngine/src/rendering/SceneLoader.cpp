#include "pch.h"
#include "rendering/SceneLoader.h"

#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "rendering/texture/TextureManager.h"
#include "rendering/device/RenderContext.h"
#include "rendering/utils/VerboseAssertion.h"
#include "rendering/graphics/Mesh.h"

using namespace std;
using namespace DirectX;

namespace
{
	XMMATRIX AiToXMMatrix(const aiMatrix4x4& aiMatrix)
	{
		return XMMATRIX(
			aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1,
			aiMatrix.a2, aiMatrix.b2, aiMatrix.c2, aiMatrix.d2,
			aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3,
			aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4
		);
	}
}

Model SceneLoader::LoadScene(const filesystem::path& filePath, ID3D11Device* device,
                             ShaderProgram&& shaderProgram)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile
	(
		filePath.string(),
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_ConvertToLeftHanded |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FlipWindingOrder
	);

	if (not scene or not scene->mRootNode)
		throw runtime_error("Could not load scene: " + filePath.string());

	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		materials.push_back(ProcessMaterial(filePath.parent_path(), scene, scene->mMaterials[i], device));

	ProcessNode(scene->mRootNode, scene, device, meshes);

	Model model{std::move(meshes), std::move(materials), device, std::move(shaderProgram)};
	return model;
}

void SceneLoader::ProcessNode(const aiNode* node, const aiScene* scene, ID3D11Device* device,
                              std::vector<Mesh>& meshesOut)
{
	const XMMATRIX transform = AiToXMMatrix(node->mTransformation);

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshesOut.push_back(ProcessMesh(mesh, device, transform));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, device, meshesOut);
	}
}

Mesh SceneLoader::ProcessMesh(const aiMesh* mesh, ID3D11Device* device, const XMMATRIX& transform)
{
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	vertices.reserve(mesh->mNumVertices);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};

		XMVECTOR pos = XMVectorSet(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
		pos = XMVector3Transform(pos, transform);
		XMStoreFloat3(&vertex.position, pos);

		if (mesh->HasNormals())
		{
			XMVECTOR normal = XMVectorSet(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.0f);
			normal = XMVector3TransformNormal(normal, transform);
			XMStoreFloat3(&vertex.normal, normal);
		}

		if (mesh->mTextureCoords[0])
			vertex.textureCoord = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			vertex.textureCoord = XMFLOAT2(0.0f, 0.0f);

		// tangents are not handled
		/*
		if (mesh->HasTangentsAndBitangents())
		{
		    vertex.tangent = XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		    vertex.bitangent = XMFLOAT3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		}
		*/

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	const UINT materialIndex = mesh->mMaterialIndex;

	return Mesh(std::move(vertices), std::move(indices), materialIndex, device);
}

Material SceneLoader::ProcessMaterial(const filesystem::path& materialPath, const aiScene* scene,
                                      const aiMaterial* material, ID3D11Device* device)
{
	Material mat;

	aiColor4D color;
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color))
		mat.ambient = XMFLOAT4(color.r, color.g, color.b, color.a);
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color))
		mat.diffuse = XMFLOAT4(color.r, color.g, color.b, color.a);
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color))
		mat.specular = XMFLOAT4(color.r, color.g, color.b, color.a);

	float shininess = 0.0f;
	if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess))
		mat.shininess = shininess;

	// Process diffuse texture
	aiString texturePath;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
	{
		if (const aiTexture* embeddedTex = scene->GetEmbeddedTexture(texturePath.C_Str()))
		{
			// Process embeded texture
			const auto shaderRessouceView = ProcessEmbededTexture(embeddedTex, device);
			mat.texture = shaderRessouceView;
		}
		else
		{
			// Process texture file
			filesystem::path absoluteTexturePath = texturePath.C_Str();
			if (!absoluteTexturePath.is_absolute())
				absoluteTexturePath = materialPath / absoluteTexturePath;

			mat.texture = textureManager.GetOrLoadFromFile(absoluteTexturePath.string(), device);
		}
	}

	return mat;
}

ComPtr<ID3D11ShaderResourceView> SceneLoader::ProcessEmbededTexture(const aiTexture* embeddedTex, ID3D11Device* device)
{
	ComPtr<ID3D11ShaderResourceView> shaderRessouceView;

	if (embeddedTex->mHeight == 0)
	{
		// Compressed data
		shaderRessouceView = textureManager.GetOrLoadFromMemory(
			reinterpret_cast<const unsigned char*>(embeddedTex->pcData),
			embeddedTex->mWidth,
			device);
	}
	else
	{
		// Non compressed data
		const size_t size = static_cast<size_t>(embeddedTex->mWidth) * embeddedTex->mHeight * 4;
		shaderRessouceView = textureManager.GetOrLoadFromMemory(
			reinterpret_cast<const unsigned char*>(embeddedTex->pcData),
			size,
			device);
	}

	return shaderRessouceView;
}
