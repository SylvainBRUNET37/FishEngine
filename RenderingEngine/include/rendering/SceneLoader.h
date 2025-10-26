#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <filesystem>
#include <vector>
#include <assimp/scene.h>
#include <DirectXMath.h>

#include "device/RenderContext.h"
#include "texture/TextureManager.h"
#include "loading/Node.h"

class SceneLoader
{
public:
	// TODO: Link shader program to mesh.
	// This method is a temporary solution, every mesh will be rendered using same shaders
	explicit SceneLoader(const ShaderProgram& shaderProgram) : shaderProgram{shaderProgram}
	{
	}

	[[nodiscard]] SceneResource LoadScene(
		const std::filesystem::path& filePath,
		ID3D11Device* device
	);

private:
	ShaderProgram shaderProgram;
	TextureManager textureManager{};
	static void ProcessNodeHierarchy(const aiNode* aiNode,
		const aiScene* scene,
		uint32_t parentIndex,
		SceneResource& sceneRes);

	static Mesh ProcessMesh(const aiMesh* mesh, ID3D11Device* device, const DirectX::XMMATRIX& transform);

	Material ProcessMaterial(const std::filesystem::path& materialPath, const aiScene* scene,
	                         const aiMaterial* material, ID3D11Device* device);
	ComPtr<ID3D11ShaderResourceView> ProcessEmbededTexture(const aiTexture* embeddedTex, ID3D11Device* device);
};

#endif
