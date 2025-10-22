#include "pch.h"
#include "rendering/shaders/ShaderProgram.h"

#include "rendering/shaders/ShaderReflector.h"

// TODO: avoid copies ?
ShaderProgram::ShaderProgram(ID3D11Device* device, const VertexShader& vertexShader, const PixelShader& pixelShader)
	: vertexShader{vertexShader},
	  pixelShader{pixelShader}
{
	const auto layout = ShaderReflector::CreateInputLayout(device, vertexShader.bytecode);

	inputLayout = ComPtr(layout);
}
