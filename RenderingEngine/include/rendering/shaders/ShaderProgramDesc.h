#ifndef SHADER_PROGRAM_DESC_H
#define SHADER_PROGRAM_DESC_H

#include "ShaderDesc.h"

template <typename... Shaders>
class ShaderProgramDesc
{
public:
	using TupleType = std::tuple<std::vector<ShaderDesc<Shaders>>...>;

	// Add a new shader description for a given shader type
	template <typename Shader>
	ShaderProgramDesc& AddDesc(std::filesystem::path path,
	                             std::string entryPoint,
	                             std::string profile)
	{
		static_assert(ContainsShaderType<Shader>(),
		              "Shader type not part of this ShaderProgramDesc template parameter pack");

		auto& shaderList = std::get<std::vector<ShaderDesc<Shader>>>(shaderDescs);
		shaderList.push_back({std::move(path), std::move(entryPoint), std::move(profile)});
		return *this;
	}

	[[nodiscard]] const TupleType& Get() const noexcept { return shaderDescs; }
	TupleType& Get() noexcept { return shaderDescs; }

private:
	TupleType shaderDescs;

	template <typename T>
	static constexpr bool ContainsShaderType()
	{
		return (std::is_same_v<T, Shaders> || ...);
	}
};

#endif
