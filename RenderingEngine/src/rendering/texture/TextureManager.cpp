#include "pch.h"
#include "rendering/texture/TextureManager.h"

#include "rendering/texture/TextureLoader.h"

using namespace std;

ComPtr<ID3D11ShaderResourceView> TextureManager::GetOrLoadFromFile(const std::string& filePath, ID3D11Device* device)
{
	const auto it = fileTextureCache.find(filePath);
	if (it != fileTextureCache.end())
		return it->second;

	auto srv = TextureLoader::LoadTextureFromFile(filePath, device);
	fileTextureCache[filePath] = srv;

	return srv;
}

ComPtr<ID3D11ShaderResourceView> TextureManager::GetOrLoadFromMemory(const unsigned char* data, const size_t size,
                                                                    ID3D11Device* device)
{
	const size_t hash = HashMemory(data, size);

	const auto it = memeoryTextureCache.find(hash);
	if (it != memeoryTextureCache.end())
		return it->second;

	auto srv = TextureLoader::LoadTextureFromMemory(data, size, device);
	memeoryTextureCache[hash] = srv;

	return srv;
}

size_t TextureManager::HashMemory(const unsigned char* data, const size_t size)
{
	constexpr hash<string_view> hasher;
	return hasher(string_view(reinterpret_cast<const char*>(data), size));
}
