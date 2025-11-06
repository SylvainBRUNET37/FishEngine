#include "pch.h"
#include "UIManager.h"

#include "rendering/texture/TextureLoader.h"

UIManager::UIManager(ID3D11Device* device) : device{device}
{
}

Sprite2D UIManager::LoadSprite(const std::string& filePath, const ResourceManager& resourceManager) const
{
	const auto texture = TextureLoader::LoadTextureFromFile(filePath, device);
	auto& shaderBank = resourceManager.GetShaderBank();

	const ShaderProgram spriteShaderProgram
	(
		device,
		shaderBank.Get<VertexShader>("shaders/SpriteVS.hlsl"),
		shaderBank.Get<PixelShader>("shaders/SpritePS.hlsl")
	);

	return Sprite2D{ spriteShaderProgram, texture, device };
}
