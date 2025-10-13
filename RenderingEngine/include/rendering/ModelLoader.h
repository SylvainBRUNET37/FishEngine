#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <filesystem>
#include <assimp/scene.h>

#include "device/GraphicsDevice.h"
#include "texture/TextureManager.h"
#include "graphics/Model.h"

class ModelLoader
{
public:
	[[nodiscard]] Model LoadModel(const std::filesystem::path& filePath,
		const GraphicsDevice* graphicsDevice, ShaderProgram&& shaderProgram);

private:
	TextureManager textureManager{};

	std::vector<Mesh> meshes;
	std::vector<Material> materials;

	void ProcessMesh(const aiMesh* mesh, const GraphicsDevice* device);
	void ProcessMaterial(const std::filesystem::path& materialPath, const aiMaterial* material, const GraphicsDevice* device);
};

#endif
