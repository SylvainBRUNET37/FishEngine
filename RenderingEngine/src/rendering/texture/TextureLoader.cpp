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
		.texture = CreateTextureView(device, texture),
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

	return CreateTextureView(device, texture);
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
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	desc.CPUAccessFlags = 0;

	return desc;
}

ComPtr<ID3D11Texture2D> TextureLoader::CreateTexture(ID3D11Device* device, const PictureData& pictureData,
	const D3D11_TEXTURE2D_DESC& desc)
{
	ComPtr<ID3D11Texture2D> texture;

	DXEssayer(device->CreateTexture2D(&desc, nullptr, &texture), "Failed to create 2D texture");
	SetDebugName(texture, "texture-in-TextureLoader");

	ID3D11DeviceContext* context = nullptr;
	device->GetImmediateContext(&context);
	context->UpdateSubresource(texture, 0, nullptr, pictureData.data, pictureData.width * 4, 0);

	ComPtr<ID3D11ShaderResourceView> srv = CreateTextureView(device, texture);
	context->GenerateMips(srv);

	return texture;
}

ComPtr<ID3D11ShaderResourceView> TextureLoader::CreateTextureView(ID3D11Device* device,
                                                                  const ComPtr<ID3D11Texture2D>& texture)
{
	ComPtr<ID3D11ShaderResourceView> textureView{};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DXEssayer(
		device->CreateShaderResourceView(texture, &srvDesc, &textureView), 
		"Failed to create texture view"
	);
	SetDebugName(textureView, "textureView-in-TextureLoader");

	return textureView;
}
