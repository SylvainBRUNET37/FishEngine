#include "pch.h"
#include "rendering/TextureManager.h"

#include "rendering/utils/VerboseAssertion.h"

using namespace std;

Texture* TextureManager::GetNewTexture(const wstring& filename, const GraphicsDevice* device)
{
    if (auto* existingTexture = GetTexture(filename))
        return existingTexture;

    auto newTexture = make_unique<Texture>(filename, device);
    Texture* texture = newTexture.get();

    vassert(texture, "Failed to create texture for file: " + string(filename.begin(), filename.end()));

    textures.push_back(std::move(newTexture));

    return texture;
}

Texture* TextureManager::GetTexture(const wstring& filename) const
{
	for (auto& texture : textures)
		if (texture->GetFilename() == filename)
			return texture.get();

	return nullptr;
}
