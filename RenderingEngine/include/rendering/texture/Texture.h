#ifndef TEXTURE_H
#define TEXTURE_H

#include "rendering/utils/ComPtr.h"

struct Texture
{
	ComPtr<ID3D11ShaderResourceView> texture;
	UINT width;
	UINT height;
};

#endif