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

};

#endif