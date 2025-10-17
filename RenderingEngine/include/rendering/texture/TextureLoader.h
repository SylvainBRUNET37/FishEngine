#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "rendering/utils/ComPtr.h"

class TextureLoader
{
public:
	[[nodiscard]] static ComPtr<ID3D11ShaderResourceView>
	LoadTextureFromFile(const std::string& filePath, ID3D11Device* device);

	[[nodiscard]] static ComPtr<ID3D11ShaderResourceView>
	LoadTextureFromMemory(const unsigned char* data, size_t size, ID3D11Device* device);

private:
	struct PictureData
	{
		int width{};
		int height{};
		int channels{};
		unsigned char* data;
	};

	[[nodiscard]] static PictureData LoadPictureFromFile(const std::string& filePath);
	[[nodiscard]] static PictureData LoadPictureFromMemory(const unsigned char* data, size_t size);

	[[nodiscard]] static D3D11_TEXTURE2D_DESC CreateTextureDesc(const PictureData& pictureData);
	[[nodiscard]] static ComPtr<ID3D11Texture2D> CreateTexture(ID3D11Device* device, const PictureData& pictureData,
	                                                           const D3D11_TEXTURE2D_DESC& desc);
	[[nodiscard]] static ComPtr<ID3D11ShaderResourceView> CreateTextureView(
		ID3D11Device* device, const ComPtr<ID3D11Texture2D>& texture, const D3D11_TEXTURE2D_DESC& desc);
};

#endif
