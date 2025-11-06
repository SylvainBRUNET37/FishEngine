#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "ResourceManager.h"
#include "rendering/graphics/Sprite.h"

class UIManager
{
public:
	explicit UIManager(ID3D11Device* device);

	[[nodiscard]] Sprite LoadSprite(const std::string& filePath, const ResourceManager& resourceManager) const;

private:
	ID3D11Device* device;

	std::unordered_map<std::string, Sprite> sprites;
};

#endif