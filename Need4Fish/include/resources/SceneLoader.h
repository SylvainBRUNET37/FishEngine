#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <filesystem>
#include <assimp/scene.h>
#include <DirectXMath.h>

#include "rendering/device/RenderContext.h"
#include "rendering/texture/TextureManager.h"
#include "SceneResource.h"

#include "json.hpp"

class ShaderBank;

class SceneLoader
{
public:
	// TODO: Link shader program to mesh.
	// This method is a temporary solution, every mesh will be rendered using same shaders
	explicit SceneLoader(ID3D11Device* device)
		: device{device}
	{
	}

	[[nodiscard]] SceneResource LoadScene(const std::filesystem::path& filePath, const ShaderBank& shaderBank);

	[[nodiscard]] Texture GetTexture(const std::string& filePath);

private:
	nlohmann::json sceneMetaData;

	TextureManager textureManager{};

	ID3D11Device* device;

	static void ProcessNodeHierarchy(const aiNode* aiNode,
	                                 const aiScene* scene,
	                                 uint32_t parentIndex,
	                                 SceneResource& sceneRes);

	void ReadSceneMetaDatas(const aiScene* scene);

	Mesh ProcessMesh(const aiMesh* mesh, const DirectX::XMMATRIX& transform) const;

	Material ProcessMaterial(const std::filesystem::path& materialPath, const aiScene* scene,
	                         const aiMaterial* material, const ShaderBank& shaderBank);
	ComPtr<ID3D11ShaderResourceView> ProcessEmbededTexture(const aiTexture* embeddedTex);

	static void ProcessLights(const aiScene* scene, SceneResource& sceneRes);
	static DirectionalLight ProcessDirectionalLights(const aiLight* light);
	static PointLight ProcessPointLights(const aiLight* light, const aiScene* scene);
};

#endif
