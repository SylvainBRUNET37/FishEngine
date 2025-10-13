#ifndef SHADER_PROGRAM_DESC_H
#define SHADER_PROGRAM_DESC_H

#include <unordered_map>

#include "Shader.h"
#include "ShaderDesc.h"
#include "ShaderUtils.h"

class LayoutDescBank
{
public:
	void Set(const std::string& name, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout)
	{
		layouts.emplace(name, layout);
	}

	[[nodiscard]] std::vector<D3D11_INPUT_ELEMENT_DESC> Get(const std::string& name)
	{
		const auto layoutit = layouts.find(name);

		if (layoutit != layouts.end())
			return layoutit->second;

		throw std::runtime_error("Could not get layout with name :" + name);
	}

private:
	std::unordered_map<std::string, std::vector<D3D11_INPUT_ELEMENT_DESC>> layouts;
};

class ShaderBank
{
public:
	template <typename Shader>
	Shader Get(const std::string& path) const
	{
		if constexpr (std::is_same_v<Shader, VertexShader>)
		{
			if (const auto it = vertexShaders.find(path); it != vertexShaders.end())
				return it->second;

			throw std::runtime_error("");
		}
		else if constexpr (std::is_same_v<Shader, PixelShader>)
		{
			if (const auto it = pixelShaders.find(path); it != pixelShaders.end())
				return it->second;

			throw std::runtime_error("");
		}

		throw std::runtime_error("");
	}

	template <typename Shader>
	void Set(const std::string& path, Shader&& shader)
	{
		if constexpr (std::is_same_v<Shader, VertexShader>)
		{
			vertexShaders.emplace(path, std::move(shader));
		}
		else if constexpr (std::is_same_v<Shader, PixelShader>)
		{
			pixelShaders.emplace(path, std::move(shader));
		}
	}

private:
	std::unordered_map<std::string, VertexShader> vertexShaders;
	std::unordered_map<std::string, PixelShader> pixelShaders;
};

template <class... Shaders>
struct ShaderProgramDesc
{
	std::tuple<std::vector<ShaderDesc<Shaders>>...> shaderDescs;

	template <typename Shader>
	ShaderProgramDesc& AddShader(std::filesystem::path path,
	                             std::string entryPoint,
	                             std::string profile)
	{
		auto& vec = std::get<std::vector<ShaderDesc<Shader>>>(shaderDescs);
		vec.push_back({std::move(path), std::move(entryPoint), std::move(profile)});
		return *this;
	}
};

template <class... Shaders>
class ShaderTest
{
public:
	void Compile(const ShaderProgramDesc<Shaders...>& descs, ID3D11Device* device)
	{
		std::apply([&](auto&... shaderVectors)
		{
			(..., ProcessShaderVector(shaderVectors, device));
		}, descs.shaderDescs);
	}

	ShaderBank shaderBank;

private:
	template <typename ShaderContainer>
	void ProcessShaderVector(ShaderContainer& container, ID3D11Device* device)
	{
		using ShaderType = ShaderContainer::value_type::ShaderType;

		for (auto& shader : container)
		{
			auto bytecode = ShaderUtils::CompileShader
			(
				shader.path.wstring(),
				shader.entryPoint,
				shader.profile
			);

			if constexpr (std::is_same_v<ShaderType, VertexShader>)
			{
				ComPtr<ID3D11VertexShader> vs;

				auto result = device->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, &vs);
				assert(SUCCEEDED(result));

				shaderBank.Set<VertexShader>(shader.path.string(), VertexShader{ vs, bytecode });
			}
			else if constexpr (std::is_same_v<ShaderType, PixelShader>)
			{
				ComPtr<ID3D11PixelShader> ps;

				auto result = device->CreatePixelShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, &ps);
				assert(SUCCEEDED(result));

				shaderBank.Set<PixelShader>(shader.path.string(), PixelShader{ ps, bytecode });
			}
		}
	}
};

#endif
