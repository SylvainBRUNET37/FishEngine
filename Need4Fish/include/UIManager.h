#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "resources/ResourceManager.h"
#include "rendering/graphics/ui/SpriteElement.h"
#include <vector>

class UIManager
{
	ID3D11Device* device;

	std::vector<SpriteElement> sprites;

public:
	explicit UIManager(ID3D11Device* device);

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

};

#endif