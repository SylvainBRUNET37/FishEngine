#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <filesystem>
#include <assimp/scene.h>

#include "TextureManager.h"
#include "shapes/Model.h"

class ModelLoader
{
public:
	// TODO: clear after loading of something
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
