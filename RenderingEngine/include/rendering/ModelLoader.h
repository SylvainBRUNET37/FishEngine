#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <assimp/scene.h>

#include "TextureManager.h"
#include "shapes/Model.h"

class ModelLoader
{
public:
	[[nodiscard]] Model LoadModel(const std::string& filename, GraphicsDevice* device,
	                                                      TextureManager* textureManager);

private:
	std::vector<Mesh> meshes;
	std::vector<Material> materials;

	void ProcessMesh(const aiMesh* mesh, const GraphicsDevice* device);
	void ProcessMaterial(const aiMaterial* material, const GraphicsDevice* device,
	                            TextureManager* textureManager);
};

#endif
