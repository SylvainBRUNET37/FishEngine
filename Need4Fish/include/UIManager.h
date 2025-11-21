#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "resources/ResourceManager.h"
#include "rendering/graphics/ui/SpriteElement.h"

class UIManager
{
	ID3D11Device* device;

	std::unordered_map<std::string, SpriteElement> sprites;

	[[nodiscard]] Sprite2D LoadSprite(const std::string& filePath) const;
	[[nodiscard]] Sprite2D LoadSprite(const std::string& filePath, float positionX, float positionY) const;
public:
	explicit UIManager(ID3D11Device* device);

	[[nodiscard]] std::vector<Sprite2D> GetSprites(const double deltaTime);

	void AddSprite(const std::string& filePath, float positionX, float positionY);
	void AddSprite(const std::string& filePath);
	void RemoveSprite(std::string& filePath);
	void Clear();

	void AddHoverSprite(const std::string& filePath, const std::string& hoverFilePath);
	void AddClickSprite(const std::string& filePath, const std::string& clickFilePath);

	void HandleClick();

};

#endif