#include "pch.h"
#include "rendering/texture/TextureLoader.h"

#include "external/stb_image.h"
#include "rendering/utils/Util.h"

Texture TextureLoader::LoadTextureFromFile(const std::string& filePath, ID3D11Device* device)
{
	const auto pictureData = LoadPictureFromFile(filePath);
	const auto textureDesc = CreateTextureDesc(pictureData);
	const auto texture = CreateTexture(device, pictureData, textureDesc);

	stbi_image_free(pictureData.data);

	return
	{
		.texture = CreateTextureView(device, texture, textureDesc),
		.width = static_cast<UINT>(pictureData.width),
		.height = static_cast<UINT>(pictureData.height)
	};
}

ComPtr<ID3D11ShaderResourceView> TextureLoader::LoadTextureFromMemory(
	const unsigned char* data, const size_t size, ID3D11Device* device)
{
	const auto pictureData = LoadPictureFromMemory(data, size);
	const auto textureDesc = CreateTextureDesc(pictureData);
	const auto texture = CreateTexture(device, pictureData, textureDesc);

	stbi_image_free(pictureData.data);

	return CreateTextureView(device, texture, textureDesc);
}

TextureLoader::PictureData TextureLoader::LoadPictureFromFile(const std::string& filePath)
{
	constexpr int DESIRED_CHANNELS = 4;
	PictureData pictureData{};

	pictureData.data = stbi_load(filePath.c_str(), &pictureData.width, &pictureData.height, &pictureData.channels, DESIRED_CHANNELS);

	return pictureData;
}

TextureLoader::PictureData TextureLoader::LoadPictureFromMemory(const unsigned char* data, const size_t size)
{
	constexpr int DESIRED_CHANNELS = 4;
	PictureData pictureData{};

	pictureData.data = stbi_load_from_memory(data, static_cast<int>(size),
		&pictureData.width, &pictureData.height, &pictureData.channels,
		DESIRED_CHANNELS);

	return pictureData;
}

D3D11_TEXTURE2D_DESC TextureLoader::CreateTextureDesc(const PictureData& pictureData)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = pictureData.width;
	desc.Height = pictureData.height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	return desc;
}

ComPtr<ID3D11Texture2D> TextureLoader::CreateTexture(ID3D11Device* device, const PictureData& pictureData,
                                                     const D3D11_TEXTURE2D_DESC& desc)
{
	ComPtr<ID3D11Texture2D> texture{};
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = pictureData.data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;

	DXEssayer(device->CreateTexture2D(&desc, &subResource, &texture), "Failed to create 2D texture");

	return texture;
}

ComPtr<ID3D11ShaderResourceView> TextureLoader::CreateTextureView(ID3D11Device* device,
                                                                  const ComPtr<ID3D11Texture2D>& texture,
                                                                  const D3D11_TEXTURE2D_DESC& desc)
{
	ComPtr<ID3D11ShaderResourceView> textureView{};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DXEssayer(device->CreateShaderResourceView(texture, &srvDesc, &textureView), "Failed to create texture view");

	return textureView;
}
