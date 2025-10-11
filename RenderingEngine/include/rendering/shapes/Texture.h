#ifndef TEXTURE_H
#define TEXTURE_H

#include "rendering/device/Device.h"
#include "rendering/utils/ComPtr.h"

class Texture
{
public:
	explicit Texture(std::wstring filename, const Device* device);

	[[nodiscard]] const std::wstring& GetFilename() const { return fileName; }
	[[nodiscard]] ID3D11ShaderResourceView* GetTexture() const { return texture; }

private:
	std::wstring fileName;
	ComPtr<ID3D11ShaderResourceView> texture;
};

#endif