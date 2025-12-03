#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <d3d11.h>

#include "resources/SceneLoader.h"
#include "rendering/shaders/ShaderBank.h"

class ResourceManager
{
public:
	explicit ResourceManager(ID3D11Device* device);

	SceneResource& LoadScene();
	[[nodiscard]] const ShaderBank& GetShaderBank() const { return shaderBank; }
	[[nodiscard]] SceneResource& GetSceneResource() { return sceneResource; }
	[[nodiscard]] Mesh& GetMesh(const UINT meshIndex) { return sceneResource.meshes[meshIndex]; }
	void ReportLiveDeviceObjects();

private:
	ID3D11Device* device;

	ShaderBank shaderBank;
	SceneLoader sceneLoader;

	SceneResource sceneResource;

	ComPtr<ID3D11Debug> debug;

	// Compile and create every shader
	void InitShaderBank();
};

#endif
