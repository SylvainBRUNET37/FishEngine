#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <filesystem>
#include <vector>
#include <assimp/scene.h>
#include <DirectXMath.h>

#include "device/RenderContext.h"
#include "texture/TextureManager.h"
#include "graphics/Model.h"

class ModelLoader
{
public:
    [[nodiscard]] Model LoadModel(
        const std::filesystem::path& filePath,
        ID3D11Device* device,
        ShaderProgram&& shaderProgram);

private:
    TextureManager textureManager{};
    static void ProcessNode(const aiNode* node, const aiScene* scene, ID3D11Device* device, std::vector<Mesh>& meshesOut);

    static Mesh ProcessMesh(const aiMesh* mesh, ID3D11Device* device, const DirectX::XMMATRIX& transform);

    Material ProcessMaterial(const std::filesystem::path& materialPath, const aiScene* scene, const aiMaterial* material, ID3D11Device* device);
	ComPtr<ID3D11ShaderResourceView> ProcessEmbededTexture(const aiTexture* embeddedTex, ID3D11Device* device);
};

#endif
