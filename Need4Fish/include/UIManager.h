#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "resources/ResourceManager.h"
#include "rendering/graphics/ui/SpriteElement.h"

class UIManager
{
	ID3D11Device* device;

	std::unordered_map<std::string, SpriteElement> sprites;

	bool clearRequested = false;

public:
	explicit UIManager(ID3D11Device* device);

	[[nodiscard]] std::vector<Sprite2D> GetSprites();

	[[nodiscard]] Sprite2D LoadSprite(const std::string& filePath) const;
	[[nodiscard]] Sprite2D LoadSprite(const std::string& filePath, float positionX, float positionY) const;
	
	void AddSprite(const std::string& filePath, float positionX, float positionY);
	void AddSprite(const std::string& filePath);
	void RemoveSprite(std::string& filePath);
	void Clear();
	void RequestClear();

	void AddHoverSprite(const std::string& filePath, const std::string& hoverFilePath);
	void AddClickSprite(const std::string& filePath, const std::string& clickFilePath);
	void AddClickFunction(const std::string& filePath, std::function<void()>);

	void HandleClick();

};

#endif