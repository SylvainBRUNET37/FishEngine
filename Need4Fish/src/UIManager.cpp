#include "pch.h"
#include "UIManager.h"
#include "Locator.h"
#include "rendering/texture/TextureLoader.h"
#include "rendering/utils/Util.h"
#include <limits>
#include <Gdiplus.h>
#include <DirectXMath.h>

using namespace Gdiplus;
using namespace DirectX;

UIManager::UIManager(ID3D11Device* device) : device{device}
{
	// Text renderer
	TextRenderer::Init();

	// create a GDI+ font for the TextRenderer
	const FontFamily family(L"Arial", nullptr);
	textFont = std::make_unique<Gdiplus::Font>(&family, 24.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

	textRenderer = std::make_unique<TextRenderer>(device, 256, 256,
		textFont.get());
}

UIManager::~UIManager()
{
	TextRenderer::Close();

}

Sprite2D UIManager::LoadSprite(const std::string& filePath) const
{
	return LoadSprite(filePath, 0.0f, 0.0f, 0.0f);
}

Sprite2D UIManager::LoadSprite(const std::string& filePath, float positionX, float positionY, float positionZ) const
{
	const auto texture = TextureLoader::LoadTextureFromFile(filePath, device);
	auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();

	const ShaderProgram spriteShaderProgram
	(
		device,
		shaderBank.Get<VertexShader>("shaders/SpriteVS.hlsl"),
		shaderBank.Get<PixelShader>("shaders/SpritePS.hlsl")
	);

	return Sprite2D{ spriteShaderProgram, texture, { positionX, positionY, positionZ }, device };
}

[[nodiscard]] std::vector<Sprite2D> UIManager::GetSprites() {
	std::vector<Sprite2D> displayedSprites{};
	for (auto& sprite : sprites)
		displayedSprites.emplace_back(sprite.UpdateAndGetDisplayedSprite());

	std::sort(displayedSprites.begin(), displayedSprites.end(),
		[](const Sprite2D& a, const Sprite2D& b)
		{
			return a.position.z > b.position.z;
		});

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
	float minZ = -FLT_MAX;
	std::function<void()> toExecute = []{};

	for (auto& sprite : sprites)
	{
		if (
			float currentZ = sprite.sprite.position.z;
			sprite.isHovered()
			&& sprite.remainingDelay <= 0.0f
			&& currentZ >= minZ
		)
		{
			sprite.remainingDelay = sprite.clickDelay;
			minZ = currentZ;
			toExecute = sprite.onClick;
			if (sprite.isCheckBox) {
				sprite.InvertBaseAndClickSprites();
			}
		}
	}
	toExecute();
}

void UIManager::EditSpritePosition(Sprite2D& sprite, float positionX, float positionY, float positionZ)
{
	sprite.position.x = positionX;
	sprite.position.y = positionY;
	sprite.position.z = positionZ;
	sprite.vertexBuffer = VertexBuffer(device, Sprite2D::ComputeVertices(sprite.position, sprite.texture));
}

static float computeXPosition(Sprite2D& sprite, const std::string alignment)
{
	const float screenWidth = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
	float newX;
	if (alignment == "left")
	{
		newX = 0.0f;
	}
	else if (alignment == "center")
	{
		newX = screenWidth / 2 - sprite.texture.width / 2;
	}
	else if (alignment == "right")
	{
		newX = screenWidth - sprite.texture.width;
	}
	else
	{
		return sprite.position.x;
	}
	return newX;
}

static float computeYPosition(Sprite2D& sprite, const std::string alignment)
{
	const float screenHeight = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));
	float newY;
	if (alignment == "left")
	{
		newY = 0.0f;
	}
	else if (alignment == "center")
	{
		newY = screenHeight / 2 - sprite.texture.height / 2;
	}
	else if (alignment == "right")
	{
		newY = screenHeight - sprite.texture.height;
	}
	else
	{
		return sprite.position.y;
	}
	return newY;
}

void UIManager::AlignSpriteX(Sprite2D& sprite, const std::string alignment)
{
	float newX = computeXPosition(sprite, alignment);
	EditSpritePosition(sprite, newX, sprite.position.y, sprite.position.z);
}

void UIManager::AlignSpriteY(Sprite2D& sprite, const std::string alignment)
{
	float newY = computeYPosition(sprite, alignment);
	EditSpritePosition(sprite, sprite.position.x, newY, sprite.position.z);
}

void UIManager::AlignSpriteXY(Sprite2D& sprite, const std::string alignmentX, const std::string alignmentY)
{
	float newX = computeXPosition(sprite, alignmentX);
	float newY = computeYPosition(sprite, alignmentY);
	EditSpritePosition(sprite, newX, newY, sprite.position.z);
}

void UIManager::TranslateSpriteX(Sprite2D& sprite, const float translation)
{
	float newX = sprite.position.x + translation;
	EditSpritePosition(sprite, newX, sprite.position.y, sprite.position.z);
}

void UIManager::TranslateSpriteY(Sprite2D& sprite, const float translation)
{
	float newY = sprite.position.y + translation;
	EditSpritePosition(sprite, sprite.position.x, newY, sprite.position.z);
}

void UIManager::TranslateSpriteXY(Sprite2D& sprite, const float translationX, const float translationY)
{
	float newX = sprite.position.x + translationX;
	float newY = sprite.position.y + translationY;
	EditSpritePosition(sprite, newX, newY, sprite.position.z);
}

void UIManager::RenderText(const std::wstring& text, ID3D11DeviceContext* context, float x, float y, float width, float height)
{
	if (!textRenderer) return;

	// Write new text into the GDI+ bitmap and update the GPU texture
	textRenderer->Ecrire(text);

	Texture textTexture(ComPtr(textRenderer->GetTextureView()), textRenderer->GetTextWidth(), textRenderer->GetTextHeigth());

	auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();

	const ShaderProgram spriteShaderProgram
	(
		device,
		shaderBank.Get<VertexShader>("shaders/SpriteVS.hlsl"),
		shaderBank.Get<PixelShader>("shaders/SpritePS.hlsl")
	);
	Sprite2D textSprite(spriteShaderProgram, textTexture, device);
	AddSprite({ textSprite });

}
