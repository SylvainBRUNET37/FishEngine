#ifndef SHADER_FACTORY_H
#define SHADER_FACTORY_H

#include "ShaderBank.h"
#include "ShaderProgramDesc.h"
#include "ShaderUtils.h"

// ===============================================
// Processor helper (specialized per shader type)
// ===============================================
template <typename ShaderType>
struct ShaderFactoryProcessor
{
	template <typename ShaderBankType, typename ShaderDesc, typename BytecodeType>
	static void Create(ShaderBankType&, ShaderDesc&, ID3D11Device*, BytecodeType&)
	{
		static_assert(sizeof(ShaderType) == 0, "Unsupported shader type in ShaderFactoryProcessor");
	}
};

// ===============================================
// Main factory template
// ===============================================
template <class... Shaders>
class ShaderFactory
{
public:
	[[nodiscard]] ShaderBank CreateShaderBank(const ShaderProgramDesc<Shaders...>& shaderDescs, ID3D11Device* device)
	{
		// Process (compile + create) every shader for the given descriptions
		std::apply([&](auto&... shaders)
		{
			(..., ProcessShaders(shaders, device)); 
		}, shaderDescs.Get());

		return std::move(shaderBank);
	}

private:
	ShaderBank shaderBank;

	template <typename ShaderContainer>
	void ProcessShaders(ShaderContainer& shaders, ID3D11Device* device)
	{
		using ShaderType = ShaderContainer::value_type::ShaderType;

		for (auto& shader : shaders)
		{
			auto bytecode = ShaderUtils::Compile(shader.path.wstring(), shader.entryPoint, shader.profile);

			ShaderFactoryProcessor<ShaderType>::Create(shaderBank, shader, device, bytecode);
		}
	}
};

// ======================================================
// Specialization for VertexShader
// ======================================================
template <>
struct ShaderFactoryProcessor<VertexShader>
{
	template <typename ShaderBankType, typename ShaderDesc, typename BytecodeType>
	static void Create(ShaderBankType& bank, ShaderDesc& shader, ID3D11Device* device, BytecodeType& bytecode)
	{
		ComPtr<ID3D11VertexShader> vs;
		auto result = device->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, &vs);
		assert(SUCCEEDED(result));

		bank.template Set<VertexShader>(shader.path.string(), VertexShader{vs, bytecode});
	}
};

// ======================================================
// Specialization for PixelShader
// ======================================================
template <>
struct ShaderFactoryProcessor<PixelShader>
{
	template <typename ShaderBankType, typename ShaderDesc, typename BytecodeType>
	static void Create(ShaderBankType& bank, ShaderDesc& shader, ID3D11Device* device, BytecodeType& bytecode)
	{
		ComPtr<ID3D11PixelShader> pixelShader;
		auto result = device->CreatePixelShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, &pixelShader);
		assert(SUCCEEDED(result));

		bank.template Set<PixelShader>(shader.path.string(), PixelShader{pixelShader, bytecode});
	}
};

#endif
