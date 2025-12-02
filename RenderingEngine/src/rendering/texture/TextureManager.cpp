#include "pch.h"
#include "rendering/texture/TextureManager.h"

#include "rendering/texture/TextureLoader.h"

using namespace std;

Texture TextureManager::GetOrLoadFromFile(const std::string& filePath, ID3D11Device* device)
{
	const auto textureIt = fileTextureCache.find(filePath);
	if (textureIt != fileTextureCache.end())
		return textureIt->second;

	auto texture = TextureLoader::LoadTextureFromFile(filePath, device);
	fileTextureCache[filePath] = texture;

	return texture;
}

ComPtr<ID3D11ShaderResourceView> TextureManager::GetOrLoadFromMemory(const unsigned char* data, const size_t size,
                                                                    ID3D11Device* device)
{
	const size_t hash = HashMemory(data, size);

	const auto textureIt = memeoryTextureCache.find(hash);
	if (textureIt != memeoryTextureCache.end())
		return textureIt->second;

	auto texture = TextureLoader::LoadTextureFromMemory(data, size, device);
	memeoryTextureCache[hash] = texture;

	return texture;
}

size_t TextureManager::HashMemory(const unsigned char* data, const size_t size)
{
	constexpr hash<string_view> hasher{};

	return hasher(string_view{ reinterpret_cast<const char*>(data), size });
}
