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
	for (auto& sprite : sprites)
		displayedSprites.emplace_back(sprite.UpdateAndGetDisplayedSprite());
	return displayedSprites;
}

void UIManager::AddSprite(const SpriteElement& sprite)
{
	sprites.emplace_back(sprite);
}

void UIManager::Clear()
{
	sprites.clear();
}

void UIManager::HandleClick() {
	std::vector<SpriteElement> spritesCopy = sprites;

	for (auto& sprite : spritesCopy)
		sprite.onClick();
}
