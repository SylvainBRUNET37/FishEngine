#ifndef SHADER_BANK_H
#define SHADER_BANK_H

#include <unordered_map>

#include "Shader.h"

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

	template <typename>
	friend struct ShaderBankAccessor;
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
		if (const auto vsIt = bank.vertexShaders.find(path); vsIt != bank.vertexShaders.end())
			return vsIt->second;
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
		if (const auto psIt = bank.pixelShaders.find(path); psIt != bank.pixelShaders.end())
			return psIt->second;
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
		if (const auto layoutIt = bank.layouts.find(path); layoutIt != bank.layouts.end())
			return layoutIt->second;
		throw std::runtime_error("Layout not found: " + path);
	}

	static void Set(ShaderBank& bank, const std::string& path, ShaderBank::Layout&& layout)
	{
		bank.layouts.emplace(path, std::move(layout));
	}
};

#endif
