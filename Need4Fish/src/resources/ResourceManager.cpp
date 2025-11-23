#include "pch.h"
#include "resources/ResourceManager.h"

#include "rendering/shaders/ShaderProgramDesc.h"
#include "rendering/shaders/ShaderFactory.h"

#include "resources/SceneLoader.h"

ResourceManager::ResourceManager(ID3D11Device* device) : device{device}, sceneLoader{device}
{
	InitShaderBank();
}

void ResourceManager::InitShaderBank()
{
	ShaderProgramDesc<VertexShader, PixelShader> shaderDescriptions;

	// Add description of each shader program of the project
	shaderDescriptions.AddDesc<VertexShader>("shaders/MainVS.hlsl", "MainVS", "vs_5_0")
		.AddDesc<PixelShader>("shaders/PhongCausticsPS.hlsl", "PhongCausticsPS", "ps_5_0")
		.AddDesc<PixelShader>("shaders/PhongWaterPS.hlsl", "PhongWaterPS", "ps_5_0")

		.AddDesc<VertexShader>("shaders/SpriteVS.hlsl", "SpriteVS", "vs_5_0")
		.AddDesc<PixelShader>("shaders/SpritePS.hlsl", "SpritePS", "ps_5_0")

		.AddDesc<VertexShader>("shaders/PostProcessVS.hlsl", "PostProcessVS", "vs_5_0")
		.AddDesc<PixelShader>("shaders/PostProcessPS.hlsl", "PostProcessPS", "ps_5_0");

	ShaderFactory<VertexShader, PixelShader> shaderFactory;
	shaderBank = shaderFactory.CreateShaderBank(shaderDescriptions, device);
} 

SceneResource& ResourceManager::LoadScene()
{
	// TODO: Link shaders to Mesh (curently, every mesh use the same shaders)
	const ShaderProgram shaderProgram
	{
		device,
		shaderBank.Get<VertexShader>("shaders/MainVS.hlsl"),
		shaderBank.Get<PixelShader>("shaders/PhongCausticsPS.hlsl"),
	};

	const std::filesystem::path filePath = "assets\\MainScene_Volet2.glb";
	sceneResource = sceneLoader.LoadScene(filePath, shaderProgram);

	return sceneResource;
}
