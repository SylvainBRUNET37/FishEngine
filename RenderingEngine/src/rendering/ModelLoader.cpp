#include "pch.h"
#include "rendering/ModelLoader.h"

#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "rendering/texture/TextureManager.h"
#include "rendering/device/GraphicsDevice.h"
#include "rendering/utils/VerboseAssertion.h"
#include "rendering/graphics/Mesh.h"

using namespace std;
using namespace DirectX;

namespace
{
    XMMATRIX AiToXMMatrix(const aiMatrix4x4& m)
    {
        return XMMATRIX(
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4
        );
    }
}

Model ModelLoader::LoadModel(const filesystem::path& filePath, const GraphicsDevice* graphicsDevice, ShaderProgram&& shaderProgram)
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

    vassert(scene && scene->mRootNode, "Could not load mesh: " + filePath.string());

    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        materials.push_back(ProcessMaterial(filePath.parent_path(), scene->mMaterials[i], graphicsDevice));

    ProcessNode(scene->mRootNode, scene, graphicsDevice, meshes);

    Model model{ std::move(meshes), std::move(materials), graphicsDevice->GetD3DDevice(), std::move(shaderProgram) };
    return model;
}

void ModelLoader::ProcessNode(const aiNode* node, const aiScene* scene, const GraphicsDevice* device, std::vector<Mesh>& meshesOut)
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

Mesh ModelLoader::ProcessMesh(const aiMesh* mesh, const GraphicsDevice* device, const XMMATRIX& transform)
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

    return Mesh(std::move(vertices), std::move(indices), materialIndex, device->GetD3DDevice());
}

Material ModelLoader::ProcessMaterial(const filesystem::path& materialPath, const aiMaterial* material, const GraphicsDevice* device)
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
        std::filesystem::path texPath = texturePath.C_Str();
        if (!texPath.is_absolute())
            texPath = materialPath / texPath;

        mat.textureFileName = texPath.string();

        const std::wstring wideFilename(texPath.wstring());
        if (const Texture* tex = textureManager.GetNewTexture(wideFilename, device))
            mat.texture = tex->GetTexture();
    }

    return mat;
}
