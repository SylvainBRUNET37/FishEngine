#ifndef SHADER_PROGRAM_DESC_H
#define SHADER_PROGRAM_DESC_H

#include <unordered_map>

#include "Shader.h"
#include "ShaderDesc.h"
#include "ShaderUtils.h"

class ShaderSystem
{
	
};

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
	[[nodiscard]] Shader Get(const std::string& path) const
	{
		if constexpr (std::is_same_v<Shader, VertexShader>)
		{
			if (const auto it = vertexShaders.find(path); it != vertexShaders.end())
				return it->second;
		}
		else if constexpr (std::is_same_v<Shader, PixelShader>)
		{
			if (const auto it = pixelShaders.find(path); it != pixelShaders.end())
				return it->second;
		}
		else
		{
			static_assert(false, "Unsupported shader type in ShaderBank::Get()");
		}

		throw std::runtime_error("Shader not found: " + path);
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
		else
		{
			static_assert(false, "Unsupported shader type in ShaderBank::Set()");
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
class ShaderFactory
{
public:
	[[nodiscard]] ShaderBank CreateShaderBank(const ShaderProgramDesc<Shaders...>& shaderDescs, ID3D11Device* device)
	{
		std::apply([&](auto&... shaders)
		{
			(..., ProcessShaders(shaders, device));
		}, shaderDescs.shaderDescs);

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
			auto bytecode = ShaderUtils::Compile
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
			else
			{
				static_assert(false, "Unsupported shader type in ShaderFactory");
			}
		}
	}
};

#endif
