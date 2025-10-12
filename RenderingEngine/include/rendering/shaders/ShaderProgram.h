#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM

#include "rendering/utils/ComPtr.h"

struct ShaderProgram
{
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11InputLayout> inputLayout;

	void Bind(ID3D11DeviceContext* context)
	{
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);
		context->IASetInputLayout(inputLayout);
	}
};

#endif