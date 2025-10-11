#include "pch.h"
#include "rendering/shapes/Texture.h"

#include "rendering/utils/DDSTextureLoader.h"
#include "rendering/utils/Util.h"
#include "resources/resource.h"

using namespace DirectX;

Texture::Texture(std::wstring filename, const Device* device)

	: fileName(std::move(filename))
{
	ID3D11Device* pDevice = device->GetD3DDevice();

	DXEssayer(CreateDDSTextureFromFile(pDevice,
	                                            fileName.c_str(),
	                                            nullptr,
	                                            &texture), DXE_FICHIERTEXTUREINTROUVABLE);
}
