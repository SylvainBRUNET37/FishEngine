#include "pch.h"
#include "UIManager.h"
#include "Locator.h"

#include "rendering/texture/TextureLoader.h"

UIManager::UIManager(ID3D11Device* device) : device{device}
{
}

Sprite2D UIManager::LoadSprite(const std::string& filePath) const 
{
	return LoadSprite(filePath, 0.0f, 0.0f);
}

Sprite2D UIManager::LoadSprite(const std::string& filePath, float positionX, float positionY) const
{
	const auto texture = TextureLoader::LoadTextureFromFile(filePath, device);
	auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();

	const ShaderProgram spriteShaderProgram
	(
		device,
		shaderBank.Get<VertexShader>("shaders/SpriteVS.hlsl"),
		shaderBank.Get<PixelShader>("shaders/SpritePS.hlsl")
	);

	return Sprite2D{ spriteShaderProgram, texture, { positionX, positionY }, device };
}

[[nodiscard]] std::vector<Sprite2D> UIManager::GetSprites() {
	std::vector<Sprite2D> displayedSprites{};
	for (const auto& pair : sprites)
		displayedSprites.emplace_back(pair.second.sprite);
	return displayedSprites;
}

void UIManager::AddSprite(const std::string& filePath, float positionX, float positionY)
{
	sprites.emplace(filePath, LoadSprite(filePath, positionX, positionY));
}

void UIManager::AddSprite(const std::string& filePath)
{
	AddSprite(filePath, 0.0f, 0.0f);
}

void UIManager::RemoveSprite(std::string& filePath)
{
	sprites.erase(filePath);
}

void UIManager::Clear()
{
	sprites.clear();
}
