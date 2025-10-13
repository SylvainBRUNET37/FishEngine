#ifndef SHADER_PROGRAM_DESC_H
#define SHADER_PROGRAM_DESC_H

#include <unordered_map>

#include "Shader.h"
#include "ShaderDesc.h"
#include "ShaderUtils.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <d3d11.h>

class ShaderBank // Contains shaders AND layouts
{
public:
    using Layout = std::vector<D3D11_INPUT_ELEMENT_DESC>;

    template <typename Shader>
    [[nodiscard]] Shader Get(const std::string& path) const
    {
        return ShaderBankAccessor<Shader>::Get(*this, path);
    }

    template <typename Shader>
    void Set(const std::string& path, Shader&& shader)
    {
        ShaderBankAccessor<Shader>::Set(*this, path, std::forward<Shader>(shader));
    }

private:
    std::unordered_map<std::string, Layout> layouts;
    std::unordered_map<std::string, VertexShader> vertexShaders;
    std::unordered_map<std::string, PixelShader> pixelShaders;

    // Friend the accessors so they can touch the private maps
    template <typename> friend struct ShaderBankAccessor;
};

// ======================================================
// Accessor helper (primary template — undefined)
// ======================================================
template <typename Shader>
struct ShaderBankAccessor
{
    [[nodiscard]] static Shader Get(const ShaderBank&, const std::string&)
    {
        static_assert(sizeof(Shader) == 0, "Unsupported shader type in ShaderBank::Get()");
        return {};
    }

    static void Set(ShaderBank&, const std::string&, Shader&&)
    {
        static_assert(sizeof(Shader) == 0, "Unsupported shader type in ShaderBank::Set()");
    }
};

// ======================================================
// Specialization for VertexShader
// ======================================================
template <>
struct ShaderBankAccessor<VertexShader>
{
    [[nodiscard]] static VertexShader Get(const ShaderBank& bank, const std::string& path)
    {
        if (const auto it = bank.vertexShaders.find(path); it != bank.vertexShaders.end())
            return it->second;
        throw std::runtime_error("VertexShader not found: " + path);
    }

    static void Set(ShaderBank& bank, const std::string& path, VertexShader&& shader)
    {
        bank.vertexShaders.emplace(path, std::move(shader));
    }
};

// ======================================================
// Specialization for PixelShader
// ======================================================
template <>
struct ShaderBankAccessor<PixelShader>
{
    [[nodiscard]] static PixelShader Get(const ShaderBank& bank, const std::string& path)
    {
        if (const auto it = bank.pixelShaders.find(path); it != bank.pixelShaders.end())
            return it->second;
        throw std::runtime_error("PixelShader not found: " + path);
    }

    static void Set(ShaderBank& bank, const std::string& path, PixelShader&& shader)
    {
        bank.pixelShaders.emplace(path, std::move(shader));
    }
};

// ======================================================
// Specialization for Layout
// ======================================================
template <>
struct ShaderBankAccessor<ShaderBank::Layout>
{
    [[nodiscard]] static ShaderBank::Layout Get(const ShaderBank& bank, const std::string& path)
    {
        if (const auto it = bank.layouts.find(path); it != bank.layouts.end())
            return it->second;
        throw std::runtime_error("Layout not found: " + path);
    }

    static void Set(ShaderBank& bank, const std::string& path, ShaderBank::Layout&& layout)
    {
        bank.layouts.emplace(path, std::move(layout));
    }
};

//

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
