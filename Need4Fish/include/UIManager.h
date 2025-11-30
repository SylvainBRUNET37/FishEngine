#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <gdiplus.h>
#include "resources/ResourceManager.h"
#include "rendering/graphics/SpriteElement.h"
#include <vector>
#include "graphics/TextRenderer.h"
#include <memory>

class UIManager
{
	ID3D11Device* device;

	std::vector<SpriteElement> sprites;

public:
	explicit UIManager(ID3D11Device* device);

	~UIManager();

	[[nodiscard]] std::vector<Sprite2D> GetSprites();

	[[nodiscard]] Sprite2D LoadSprite(const std::string& filePath) const;
	[[nodiscard]] Sprite2D LoadSprite(const std::string& filePath, float positionX, float positionY, float positionZ) const;

	void AddSprite(const SpriteElement& sprite);
	void Clear();
	void HandleClick();

	void EditSpritePosition(Sprite2D& sprite, float positionX, float positionY, float positionZ);
	void AlignSpriteX(Sprite2D& sprite, const std::string alignment);
	void AlignSpriteY(Sprite2D& sprite, const std::string alignment);
	void AlignSpriteXY(Sprite2D& sprite, const std::string alignmentX, const std::string alignmentY);
	void TranslateSpriteX(Sprite2D& sprite, const float translation);
	void TranslateSpriteY(Sprite2D& sprite, const float translation);
	void TranslateSpriteXY(Sprite2D& sprite, const float translationX, const float translationY);

	void RenderText(const std::wstring& text, ID3D11DeviceContext* context, float x, float y, float width, float height);

private:
	// Text rendering
	TextRenderer* textRenderer;
	Gdiplus::Font* textFont;

};

#endif