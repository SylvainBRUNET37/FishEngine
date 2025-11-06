#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>
#include <unordered_map>
#include <d3d11.h>

#include "Texture.h"
#include "rendering/utils/ComPtr.h"

class TextureManager
{
public:
    [[nodiscard]] ComPtr<ID3D11ShaderResourceView>
        GetOrLoadFromMemory(const unsigned char* data, size_t size, ID3D11Device* device);

    [[nodiscard]] Texture
        GetOrLoadFromFile(const std::string& filePath, ID3D11Device* device);

private:
	std::unordered_map<std::string, Texture> fileTextureCache;
	std::unordered_map<size_t, ComPtr<ID3D11ShaderResourceView>> memeoryTextureCache;

	static size_t HashMemory(const unsigned char* data, size_t size);
};

#endif