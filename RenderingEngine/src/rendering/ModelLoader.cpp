#include "pch.h"
#include "rendering/ModelLoader.h"

#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "rendering/shaders/ShaderManager.h"
#include "rendering/TextureManager.h"
#include "rendering/device/GraphicsDevice.h"
#include "rendering/utils/VerboseAssertion.h"
#include "rendering/shapes/Mesh.h"

using namespace std;
using namespace DirectX;

Model ModelLoader::LoadModel(const filesystem::path& filePath, const GraphicsDevice* graphicsDevice, TextureManager* textureManager)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile
	(
		filePath.string(),
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_ConvertToLeftHanded |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices
	);

	vassert(scene && scene->mRootNode, "Could not load mesh: " + filePath.string());

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		ProcessMesh(scene->mMeshes[i], graphicsDevice);

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		ProcessMaterial(filePath.parent_path(), scene->mMaterials[i], graphicsDevice, textureManager);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const unordered_set<ShaderDesc> shaderDescs =
	{
		{"shaders/MiniPhongVS.hlsl", "MiniPhongVS", "vs_5_0", ShaderStage::Vertex},
		{"shaders/MiniPhongPS.hlsl", "MiniPhongPS", "ps_5_0", ShaderStage::Pixel},
	};

	ShaderProgram shaderProgram = ShaderManager::CreateShaderProgram(graphicsDevice->GetD3DDevice(), shaderDescs, layoutDesc);

	auto model = Model{std::move(meshes), std::move(materials), graphicsDevice->GetD3DDevice(), std::move(shaderProgram)};

	meshes.clear();
	materials.clear();

	return model;
}

void ModelLoader::ProcessMesh(const aiMesh* mesh, const GraphicsDevice* device)
{
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	// Process vertices
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};
		vertex.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		if (mesh->HasNormals())
			vertex.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->mTextureCoords[0])
			vertex.textureCoord = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			vertex.textureCoord = XMFLOAT2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// Process indices
	constexpr unsigned int TRIANGLE_SIDE_NUMBER = 3;
	indices.reserve(mesh->mNumFaces * TRIANGLE_SIDE_NUMBER);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	const UINT materialIndex = mesh->mMaterialIndex;

	meshes.push_back(Mesh(std::move(vertices), std::move(indices), materialIndex, device->GetD3DDevice()));
}

void ModelLoader::ProcessMaterial(const filesystem::path& materialPath, const aiMaterial* material, const GraphicsDevice* device, TextureManager* textureManager)
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

	aiString texturePath;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
	{
		const std::string texName = texturePath.C_Str();

		// Combine folder + filename
		const std::filesystem::path fullPath = materialPath / texName;

		mat.textureFileName = fullPath.string();

		// Convert to wide string
		const std::wstring wideFilename(fullPath.wstring());

		if (const Texture* tex = textureManager->GetNewTexture(wideFilename, device))
			mat.texture = tex->GetTexture();
	}

	materials.push_back(std::move(mat));
}
