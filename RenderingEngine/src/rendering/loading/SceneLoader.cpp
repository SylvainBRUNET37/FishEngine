#include "pch.h"
#include "rendering/loading/SceneLoader.h"

#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "rendering/texture/TextureManager.h"
#include "rendering/device/RenderContext.h"
#include "rendering/utils/VerboseAssertion.h"
#include "rendering/graphics/Mesh.h"
#include "rendering/graphics/Material.h"
#include "rendering/loading/Node.h"

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

SceneResource SceneLoader::LoadScene(const filesystem::path& filePath, ID3D11Device* device)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		filePath.string(),
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices
	);

	if (!scene || !scene->mRootNode)
		throw runtime_error("Could not load scene: " + filePath.string());

	SceneResource sceneRes;
	sceneRes.name = filePath.stem().string();

	// Load materials
	sceneRes.materials.reserve(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		sceneRes.materials.push_back(ProcessMaterial(filePath.parent_path(), scene, scene->mMaterials[i], device));

	// Load meshes
	sceneRes.meshes.reserve(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		sceneRes.meshes.push_back(ProcessMesh(scene->mMeshes[i], device, XMMatrixIdentity()));

	// Build node hierarchy
	sceneRes.nodes.reserve(scene->mNumMeshes); // rough estimate
	ProcessNodeHierarchy(scene->mRootNode, scene, UINT32_MAX, sceneRes);

	return sceneRes;
}

void SceneLoader::ProcessNodeHierarchy(
	const aiNode* aiNode,
	const aiScene* scene,
	const uint32_t parentIndex,
	SceneResource& sceneRes)
{
	Node node;
	Transform transform;
	const XMMATRIX nodeMatrix = AiToXMMatrix(aiNode->mTransformation);

	// Decompose the matrix
	XMVECTOR scale, rotationQuat, translation;
	if (XMMatrixDecompose(&scale, &rotationQuat, &translation, nodeMatrix))
	{
		// Store decomposed components
		XMStoreFloat3(&transform.scale, scale);
		XMStoreFloat4(&transform.rotation, rotationQuat);
		XMStoreFloat3(&transform.position, translation);

		const XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);
		const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuat);
		const XMMATRIX translationMatrix = XMMatrixTranslationFromVector(translation);

		// Construct world matrix
		transform.world = scaleMatrix * rotationMatrix * translationMatrix;
	}
	else
		transform.world = nodeMatrix;

	node.parentIndex = parentIndex;
	node.name = aiNode->mName.C_Str();
	node.transform = transform;

	// If this node has a mesh, store its index
	if (aiNode->mNumMeshes > 0)
		node.meshIndex = aiNode->mMeshes[0]; // assuming one mesh per node
	else
		node.meshIndex = UINT32_MAX;

	// Add this node to scene and get its index
	const uint32_t currentIndex = static_cast<uint32_t>(sceneRes.nodes.size());
	sceneRes.nodes.push_back(node);

	// Recursively process children
	for (unsigned int i = 0; i < aiNode->mNumChildren; i++)
	{
		uint32_t childIndexBefore = static_cast<uint32_t>(sceneRes.nodes.size());
		ProcessNodeHierarchy(aiNode->mChildren[i], scene, currentIndex, sceneRes);
		sceneRes.nodes[currentIndex].children.push_back(childIndexBefore);
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

Material SceneLoader::ProcessMaterial(
	const filesystem::path& materialPath,
	const aiScene* scene,
	const aiMaterial* material,
	ID3D11Device* device)
{
	static constexpr int materialCbRegisterNumber = 2;
	Material mat{ device, shaderProgram, materialCbRegisterNumber };

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
		if (const aiTexture* embeddedTex = scene->GetEmbeddedTexture(texturePath.C_Str()))
		{
			mat.texture = ProcessEmbededTexture(embeddedTex, device);
		}
		else
		{
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
		shaderRessouceView = textureManager.GetOrLoadFromMemory(
			reinterpret_cast<const unsigned char*>(embeddedTex->pcData),
			embeddedTex->mWidth,
			device);
	}
	else
	{
		const size_t size = static_cast<size_t>(embeddedTex->mWidth) * embeddedTex->mHeight * 4;
		shaderRessouceView = textureManager.GetOrLoadFromMemory(
			reinterpret_cast<const unsigned char*>(embeddedTex->pcData),
			size,
			device);
	}

	return shaderRessouceView;
}
