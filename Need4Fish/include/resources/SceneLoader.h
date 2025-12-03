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
	explicit SceneLoader(ID3D11Device* device)
		: device{device}
	{
	}

	[[nodiscard]] SceneResource LoadScene(const std::filesystem::path& filePath, ShaderBank& shaderBank);

	[[nodiscard]] Texture GetTexture(const std::string& filePath);

private:
	nlohmann::json sceneMetaData;

	// string = name, int = indice in SceneResource node list
	// It is used to assign lights to the correct node
	std::unordered_map<std::string, uint32_t> nodes;

	TextureManager textureManager{};

	ID3D11Device* device;

	void ProcessNodeHierarchy(const aiNode* aiNode,
	                                 const aiScene* scene,
	                                 uint32_t parentIndex,
	                                 SceneResource& sceneRes);

	void ReadSceneMetaDatas(const aiScene* scene);

	Mesh ProcessMesh(const aiMesh* mesh, const DirectX::XMMATRIX& transform) const;

	Material ProcessMaterial(const std::filesystem::path& materialPath, const aiScene* scene,
	                         const aiMaterial* material, ShaderBank& shaderBank);
	ComPtr<ID3D11ShaderResourceView> ProcessEmbededTexture(const aiTexture* embeddedTex);

	void ProcessLights(const aiScene* scene, SceneResource& sceneRes);
	static DirectionalLight ProcessDirectionalLights(const aiLight* light);
	PointLight ProcessPointLights(const aiLight* light, const aiScene* scene);
};

#endif
