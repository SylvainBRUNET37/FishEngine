#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>

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

// Used for caching shader programs
struct ShaderProgramKey
{
	std::string vsPath;
	std::string psPath;

	bool operator==(const ShaderProgramKey& other) const noexcept
	{
		return vsPath == other.vsPath && psPath == other.psPath;
	}
};

template <>
struct std::hash<ShaderProgramKey>
{
	size_t operator()(const ShaderProgramKey& shaderProgramKey) const noexcept
	{
		const size_t h1 = std::hash<std::string>{}(shaderProgramKey.vsPath);
		const size_t h2 = std::hash<std::string>{}(shaderProgramKey.psPath);

		return h1 ^ h2 << 1;
	}
};

#endif
