#ifndef SHADER_H
#define SHADER_H

#include <d3d11.h>

#include "rendering/utils/ComPtr.h"

struct VertexShader
{
	ComPtr<ID3D11VertexShader> shader;
	ComPtr<ID3DBlob> bytecode;
};

struct PixelShader
{
	ComPtr<ID3D11PixelShader> shader;
	ComPtr<ID3DBlob> bytecode;
};

#endif
