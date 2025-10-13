#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include "ShaderDesc.h"
#include "ShaderProgramDesc.h"

class ShaderBuilder
{
public:
	static ShaderBank CreateShaderProgram(ID3D11Device* device)
	{
		ShaderProgramDesc<VertexShader, PixelShader> desc;

		desc.AddShader<VertexShader>("shaders/MiniPhongVS.hlsl", "MiniPhongVS", "vs_5_0")
		    .AddShader<PixelShader>("shaders/MiniPhongPS.hlsl", "MiniPhongPS", "ps_5_0");

		ShaderTest<VertexShader, PixelShader> test;
		test.Compile(desc, device);

		const auto& shaders = test.shaderBank;

		auto a = shaders.Get<PixelShader>("shaders/MiniPhongPS.hlsl");

		return test.shaderBank;
	}
};

#endif
