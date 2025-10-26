#include "ResourceManager.h"

#include "rendering/shaders/ShaderProgramDesc.h"
#include "rendering/shaders/ShaderFactory.h"

#include "rendering/SceneLoader.h"

ResourceManager::ResourceManager(ID3D11Device* device) : device{device}
{
	InitShaderBank();
}

void ResourceManager::InitShaderBank()
{
	ShaderProgramDesc<VertexShader, PixelShader> desc;

	desc.AddDesc<VertexShader>("shaders/MiniPhongVS.hlsl", "MiniPhongVS", "vs_5_0")
		.AddDesc<PixelShader>("shaders/MiniPhongPS.hlsl", "MiniPhongPS", "ps_5_0");

	ShaderFactory<VertexShader, PixelShader> shaderFactory;
	shaderBank = shaderFactory.CreateShaderBank(desc, device);
} 

SceneResource ResourceManager::LoadScene() const
{
	const ShaderProgram shaderProgram
	{
		device,
		shaderBank.Get<VertexShader>("shaders/MiniPhongVS.hlsl"),
		shaderBank.Get<PixelShader>("shaders/MiniPhongPS.hlsl"),
	};

	const std::filesystem::path filePath = "assets\\fiat.glb";

	SceneLoader modelLoader{ shaderProgram };
	return modelLoader.LoadScene(filePath, device);
}
