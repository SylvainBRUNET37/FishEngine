#ifndef SHADER_BANK_H
#define SHADER_BANK_H

#include <stdexcept>
#include <unordered_map>

#include "Shader.h"

class ShaderBank
{
public:
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

	// Assume user use only vertex and pixel shader
	[[nodiscard]] std::shared_ptr<ShaderProgram> GetOrCreateShaderProgram(
		ID3D11Device* device,
		const std::string& vsPath,
		const std::string& psPath)
	{
		const ShaderProgramKey key{ vsPath, psPath };

		// Return cached ShaderProgram if already stored
		if (const auto shaderProgramIt = shaderPrograms.find(key); shaderProgramIt != shaderPrograms.end())
			return shaderProgramIt->second;

		const auto& vs = Get<VertexShader>(vsPath);
		const auto& ps = Get<PixelShader>(psPath);

		auto shaderProgram = std::make_shared<ShaderProgram>(device, vs, ps);

		shaderPrograms.emplace(key, shaderProgram);
		return shaderProgram;
	}

private:
	std::unordered_map<std::string, VertexShader> vertexShaders;
	std::unordered_map<std::string, PixelShader> pixelShaders;

	std::unordered_map<ShaderProgramKey, std::shared_ptr<ShaderProgram>> shaderPrograms;

	template <typename>
	friend struct ShaderBankAccessor;
};

// ======================================================
// Accessor helper (base template is undefined)
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

#endif
