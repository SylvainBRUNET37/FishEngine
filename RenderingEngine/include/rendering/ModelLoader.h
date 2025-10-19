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
        const RenderContext* graphicsDevice,
        ShaderProgram&& shaderProgram);

private:
    TextureManager textureManager{};
    static void ProcessNode(const aiNode* node, const aiScene* scene, const RenderContext* device, std::vector<Mesh>& meshesOut);

    static Mesh ProcessMesh(const aiMesh* mesh, const RenderContext* device, const DirectX::XMMATRIX& transform);

    Material ProcessMaterial(const std::filesystem::path& materialPath, const aiScene* scene, const aiMaterial* material, const RenderContext* device);
	ComPtr<ID3D11ShaderResourceView> ProcessEmbededTexture(const aiTexture* embeddedTex, const RenderContext* device);
};

#endif
