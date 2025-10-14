#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <filesystem>
#include <vector>
#include <assimp/scene.h>
#include <DirectXMath.h>

#include "device/GraphicsDevice.h"
#include "texture/TextureManager.h"
#include "graphics/Model.h"

class ModelLoader
{
public:
    [[nodiscard]] Model LoadModel(
        const std::filesystem::path& filePath,
        const GraphicsDevice* graphicsDevice,
        ShaderProgram&& shaderProgram);

private:
    TextureManager textureManager{};
    void ProcessNode(const aiNode* node, const aiScene* scene, const GraphicsDevice* device, std::vector<Mesh>& meshesOut);

    static Mesh ProcessMesh(const aiMesh* mesh, const GraphicsDevice* device, const DirectX::XMMATRIX& transform);

    Material ProcessMaterial(const std::filesystem::path& materialPath, const aiMaterial* material, const GraphicsDevice* device);
};

#endif // MODEL_LOADER_H
