#include "pch.h"
#include "rendering/shaders/ShaderProgram.h"

ShaderProgram::ShaderProgram(ID3D11Device* device, const VertexShader& vertexShader_, const PixelShader& pixelShader_,
                             const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout_)
	: vertexShader{vertexShader_.shader},
	  pixelShader{pixelShader_.shader}
{
	const HRESULT result = device->CreateInputLayout(layout_.data(), layout_.size(),
	                                                 vertexShader_.bytecode->GetBufferPointer(),
	                                                 vertexShader_.bytecode->GetBufferSize(),
	                                                 &inputLayout);

	assert(SUCCEEDED(result));
}
