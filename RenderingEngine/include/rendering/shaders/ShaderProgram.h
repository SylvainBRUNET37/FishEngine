#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Shader.h"
#include "rendering/utils/ComPtr.h"

class ShaderProgram
{
public:
	explicit ShaderProgram(ID3D11Device* device,
	                       const VertexShader& vertexShader_,
	                       const PixelShader& pixelShader_,
	                       const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout_);

	void Bind(ID3D11DeviceContext* context)
	{
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);
		context->IASetInputLayout(inputLayout);
	}

private:
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11InputLayout> inputLayout;
};

#endif
