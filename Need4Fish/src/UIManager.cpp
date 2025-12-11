#include "pch.h"
#include "UIManager.h"
#include "Locator.h"
#include "rendering/texture/TextureLoader.h"
#include <Gdiplus.h>
#include <DirectXMath.h>

using namespace Gdiplus;
using namespace DirectX;

UIManager::UIManager(RenderContext* renderContext) : device{ renderContext->GetDevice() }, renderContext{ std::move(renderContext) }
{
	// Text renderer
	TextRenderer::Init();

	// create a GDI+ font for the TextRenderer
	const FontFamily family(L"Arial", nullptr);
	textFont = new Font(&family, 24.0f, FontStyleRegular, UnitPixel);

	textRenderer = new TextRenderer(device, 300, 100, textFont);

	textTexture.texture = textRenderer->GetTextureView();
	textTexture.width = textRenderer->GetTextWidth();
	textTexture.height = textRenderer->GetTextHeigth();

	auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();

	textSprite = new Sprite2D
	{
		shaderBank.GetOrCreateShaderProgram(device, "shaders/SpriteVS.hlsl", "shaders/SpritePS.hlsl"),
		textTexture, 
		device
	};
	AddSprite({*textSprite});
}

UIManager::~UIManager()
{
	delete textRenderer;
	delete textFont;
	delete textSprite;
	TextRenderer::Close();
}

Sprite2D UIManager::LoadSprite(const std::string& filePath) const
{
	return LoadSprite(filePath, 0.0f, 0.0f, 0.0f);
}

Sprite2D UIManager::LoadSprite(const std::string& filePath, const float positionX, const float positionY, const float positionZ) const
{
	const auto texture = TextureLoader::LoadTextureFromFile(filePath, device);
	auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();

	return Sprite2D
	{
		shaderBank.GetOrCreateShaderProgram(device, "shaders/SpriteVS.hlsl", "shaders/SpritePS.hlsl"),
		texture, 
		{positionX, positionY, positionZ},
		device
	};
}

[[nodiscard]] std::vector<Sprite2D> UIManager::GetSprites()
{
	std::vector<Sprite2D> displayedSprites{};
	displayedSprites.reserve(sprites.size());
	for (auto& sprite : sprites)
		displayedSprites.emplace_back(sprite.UpdateAndGetDisplayedSprite(renderContext->GetScreenStartX(), renderContext->GetScreenStartY()));

	std::ranges::sort(displayedSprites,
	                  [](const Sprite2D& a, const Sprite2D& b)
	                  {
		                  return a.position.z < b.position.z;
	                  });

	return displayedSprites;
}

void UIManager::AddSprite(const SpriteElement& sprite)
{
	auto sprite_ = sprite;
	UpdateAlignSpriteXY(*renderContext, sprite_);
	sprites.emplace_back(sprite_);
}

void UIManager::Clear()
{
	sprites.clear();

	// Readd the text sprite since it has to be on the screen during the entire game
	AddSprite({*textSprite}); 
}

void UIManager::HandleClick()
{
	float minZ = -FLT_MAX;
	std::function<void()> toExecute = []
	{
	};

	for (auto& sprite : sprites)
	{
		if (
			float currentZ = sprite.sprite.position.z;
			sprite.isHovered(renderContext->GetScreenStartX(), renderContext->GetScreenStartY())
			&& sprite.remainingDelay <= 0.0f
			&& currentZ >= minZ
		)
		{
			sprite.remainingDelay = sprite.clickDelay;
			minZ = currentZ;
			toExecute = sprite.onClick;
			if (sprite.isCheckBox)
			{
				sprite.InvertBaseAndClickSprites();
			}
		}
	}
	toExecute();
}

void UIManager::EditSpritePosition(Sprite2D& sprite, const float positionX, const float positionY,
                                   const float positionZ) const
{
	sprite.position.x = positionX;
	sprite.position.y = positionY;
	sprite.position.z = positionZ;
	sprite.vertexBuffer = VertexBuffer(device, Sprite2D::ComputeVertices(sprite.position, sprite.texture));
}

static float computeXPosition(const RenderContext& renderContext, const Sprite2D& sprite, const std::string& alignment)
{
	const float screenWidth = static_cast<float>(renderContext.GetScreenWidth());
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

static float computeYPosition(const RenderContext& renderContext, const Sprite2D& sprite, const std::string& alignment)
{
	const float screenHeight = static_cast<float>(renderContext.GetScreenHeight());
	float newY;
	if (alignment == "top")
	{
		newY = 0.0f;
	}
	else if (alignment == "center")
	{
		newY = screenHeight / 2 - sprite.texture.height / 2;
	}
	else if (alignment == "bottom")
	{
		newY = screenHeight - sprite.texture.height;
	}
	else
	{
		return sprite.position.y;
	}
	return newY;
}

void UIManager::AlignSpriteX(Sprite2D& sprite, const std::string& alignment)
{
	const float newX = computeXPosition(*renderContext, sprite, alignment);
	EditSpritePosition(sprite, newX, sprite.position.y, sprite.position.z);
}

void UIManager::UpdateAlignSpriteX(const RenderContext& renderContext, SpriteElement& sprite)
{
	const float newX = computeXPosition(renderContext, sprite.sprite, sprite.alignX);
	EditSpritePosition(sprite.sprite, newX + sprite.offsetX, sprite.sprite.position.y, sprite.sprite.position.z);
}

void UIManager::UpdateAlignSpriteY(const RenderContext& renderContext, SpriteElement& sprite)
{
	const float newY = computeXPosition(renderContext, sprite.sprite, sprite.alignX);
	EditSpritePosition(sprite.sprite, sprite.sprite.position.x, newY + sprite.offsetY, sprite.sprite.position.z);
}

void UIManager::UpdateAlignSpriteXY(const RenderContext& renderContext, SpriteElement& sprite)
{
	auto updatePos = [&](Sprite2D& sprite_) {
		const float newX = computeXPosition(renderContext, sprite_, sprite.alignX);
		const float newY = computeYPosition(renderContext, sprite_, sprite.alignY);
		EditSpritePosition(sprite_, newX + sprite.offsetX, newY + sprite.offsetY, sprite_.position.z);
		};

	updatePos(sprite.sprite);

	if (sprite.hoverSprite.has_value())
		updatePos(sprite.hoverSprite.value());

	if (sprite.clickSprite.has_value())
		updatePos(sprite.clickSprite.value());

	if (sprite.clickHoverSprite.has_value())
		updatePos(sprite.clickHoverSprite.value());
}

void UIManager::AlignSpriteY(Sprite2D& sprite, const std::string& alignment)
{
	const float newY = computeYPosition(*renderContext, sprite, alignment);
	EditSpritePosition(sprite, sprite.position.x, newY, sprite.position.z);
}

void UIManager::AlignSpriteXY(Sprite2D& sprite, const std::string& alignmentX, const std::string& alignmentY)
{
	const float newX = computeXPosition(*renderContext, sprite, alignmentX);
	const float newY = computeYPosition(*renderContext, sprite, alignmentY);
	EditSpritePosition(sprite, newX, newY, sprite.position.z);
}

void UIManager::TranslateSpriteX(Sprite2D& sprite, const float translation)
{
	const float newX = sprite.position.x + translation;
	EditSpritePosition(sprite, newX, sprite.position.y, sprite.position.z);
}

void UIManager::TranslateSpriteY(Sprite2D& sprite, const float translation)
{
	const float newY = sprite.position.y + translation;
	EditSpritePosition(sprite, sprite.position.x, newY, sprite.position.z);
}

void UIManager::TranslateSpriteXY(Sprite2D& sprite, const float translationX, const float translationY)
{
	const float newX = sprite.position.x + translationX;
	const float newY = sprite.position.y + translationY;
	EditSpritePosition(sprite, newX, newY, sprite.position.z);
}

void UIManager::RenderText(const std::wstring& text) const
{
	if (!textRenderer) return;

	// Write new text into the GDI+ bitmap and update the GPU texture
	textRenderer->Ecrire(text);
}
