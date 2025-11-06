#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <d3d11.h>

#include "rendering/loading/Node.h"
#include "rendering/shaders/ShaderBank.h"

class ResourceManager
{
public:
	// TODO: not public !
	ShaderBank shaderBank;
	ID3D11Device* device;

	explicit ResourceManager(ID3D11Device* device);

	[[nodiscard]] SceneResource LoadScene() const;

private:

	// Compile and create every shader
	void InitShaderBank();
};

#endif
