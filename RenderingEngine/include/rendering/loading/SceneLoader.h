#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <filesystem>
#include <assimp/scene.h>
#include <DirectXMath.h>

#include "rendering/device/RenderContext.h"
#include "rendering/texture/TextureManager.h"
#include "Node.h"

class SceneLoader
{
public:
	// TODO: Link shader program to mesh.
	// This method is a temporary solution, every mesh will be rendered using same shaders
	explicit SceneLoader(ID3D11Device* device)
		: device{device}
	{
	}

	[[nodiscard]] SceneResource LoadScene(const std::filesystem::path& filePath, const ShaderProgram& shaderProgram);

	[[nodiscard]] Texture GetTexture(const std::string& filePath);

private:
	TextureManager textureManager{};

	ID3D11Device* device;

	static void ProcessNodeHierarchy(const aiNode* aiNode,
	                                 const aiScene* scene,
	                                 uint32_t parentIndex,
	                                 SceneResource& sceneRes);

	Mesh ProcessMesh(const aiMesh* mesh, const DirectX::XMMATRIX& transform) const;

	Material ProcessMaterial(const std::filesystem::path& materialPath, const aiScene* scene,
	                         const aiMaterial* material, const ShaderProgram& shaderProgram);
	ComPtr<ID3D11ShaderResourceView> ProcessEmbededTexture(const aiTexture* embeddedTex);

	static void ProcessLights(const aiScene* scene, SceneResource& sceneRes);
	static DirectionalLight ProcessDirectionalLights(const aiLight* light);
	static PointLight ProcessPointLights(const aiLight* light, const aiScene* scene);
};

#endif
