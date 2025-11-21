#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "resources/ResourceManager.h"
#include "rendering/graphics/Sprite2D.h"

class UIManager
{
	struct SpriteElement {
		Sprite2D sprite;
		// TODO:
		// optional OnHover ?
		// optional OnClick ?

		SpriteElement(const Sprite2D& s) : sprite(s) {}
	};

public:
	explicit UIManager(ID3D11Device* device);

	[[nodiscard]] std::vector<Sprite2D> GetSprites();

	void AddSprite(const std::string& filePath, float positionX, float positionY);
	void AddSprite(const std::string& filePath);
	void RemoveSprite(std::string& filePath);
	void Clear();

private:
	ID3D11Device* device;

	std::unordered_map<std::string, SpriteElement> sprites;

	[[nodiscard]] Sprite2D LoadSprite(const std::string& filePath) const;
	[[nodiscard]] Sprite2D LoadSprite(const std::string& filePath, float positionX, float positionY) const;
	
};

#endif