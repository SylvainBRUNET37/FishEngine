#include "pch.h"
#include "ResourceManager.h"

#include "rendering/shaders/ShaderProgramDesc.h"
#include "rendering/shaders/ShaderFactory.h"

#include "rendering/loading/SceneLoader.h"

ResourceManager::ResourceManager(ID3D11Device* device) : device{device}, sceneLoader{device}
{
	InitShaderBank();
}

void ResourceManager::InitShaderBank()
{
	ShaderProgramDesc<VertexShader, PixelShader> desc;

	// Add description of each shader program of the project
	desc.AddDesc<VertexShader>("shaders/MiniPhongVS.hlsl", "MiniPhongVS", "vs_5_0")
		.AddDesc<PixelShader>("shaders/MiniPhongPS.hlsl", "MiniPhongPS", "ps_5_0")
		.AddDesc<VertexShader>("shaders/SpriteVS.hlsl", "SpriteVS", "vs_5_0")
		.AddDesc<PixelShader>("shaders/SpritePS.hlsl", "SpritePS", "ps_5_0");

	ShaderFactory<VertexShader, PixelShader> shaderFactory;
	shaderBank = shaderFactory.CreateShaderBank(desc, device);
} 

SceneResource ResourceManager::LoadScene()
{
	// TODO: Link shaders to Mesh (curently, every mesh use the same shaders)
	const ShaderProgram shaderProgram
	{
		device,
		shaderBank.Get<VertexShader>("shaders/MiniPhongVS.hlsl"),
		shaderBank.Get<PixelShader>("shaders/MiniPhongPS.hlsl"),
	};

	const std::filesystem::path filePath = "assets\\MainScene_Volet2.glb";

	return sceneLoader.LoadScene(filePath, shaderProgram);
}
