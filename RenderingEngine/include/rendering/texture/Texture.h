#ifndef TEXTURE_H
#define TEXTURE_H

#include "rendering/shaders/Shader.h"
#include "rendering/utils/ComPtr.h"

struct Texture
{
	ComPtr<ID3D11ShaderResourceView> texture;
	UINT width;
	UINT height;
};

#endif