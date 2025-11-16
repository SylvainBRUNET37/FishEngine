#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "resources/ResourceManager.h"
#include "rendering/graphics/Sprite2D.h"

class UIManager
{
public:
	explicit UIManager(ID3D11Device* device);

	[[nodiscard]] Sprite2D LoadSprite(const std::string& filePath, const ResourceManager& resourceManager) const;

private:
	ID3D11Device* device;

	std::unordered_map<std::string, Sprite2D> sprites;
};

#endif