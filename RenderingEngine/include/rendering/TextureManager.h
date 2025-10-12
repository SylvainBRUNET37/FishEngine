#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "shapes/Texture.h"

class TextureManager
{
public:
	[[nodiscard]] Texture* GetNewTexture(const std::wstring& filename, const GraphicsDevice* device);

	[[nodiscard]] Texture* GetTexture(const std::wstring& filename) const;

private:
	std::vector<std::unique_ptr<Texture>> textures;
};

#endif