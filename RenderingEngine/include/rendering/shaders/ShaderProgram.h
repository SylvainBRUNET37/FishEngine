#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Shader.h"
#include "rendering/utils/ComPtr.h"

class ShaderProgram
{
public:
	explicit ShaderProgram(ID3D11Device* device,
	                       const VertexShader& vertexShader,
	                       const PixelShader& pixelShader);

	void Bind(ID3D11DeviceContext* context)
	{
		context->VSSetShader(vertexShader.shader, nullptr, 0);
		context->PSSetShader(pixelShader.shader, nullptr, 0);
		context->IASetInputLayout(inputLayout);
	}

private:
	VertexShader vertexShader;
	PixelShader pixelShader;
	ComPtr<ID3D11InputLayout> inputLayout;
};

#endif
