#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <d3d11.h>

#include "rendering/loading/Node.h"
#include "rendering/shaders/ShaderBank.h"

class ResourceManager
{
public:
	ShaderBank shaderBank;

	explicit ResourceManager(ID3D11Device* device);

	[[nodiscard]] SceneResource LoadScene() const;

private:
	ID3D11Device* device;

	// Compile and create every shader
	void InitShaderBank();
};

#endif
