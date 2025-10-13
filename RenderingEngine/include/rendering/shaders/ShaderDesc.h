#ifndef SHADER_DESC_H
#define SHADER_DESC_H

#include <filesystem>

template <class Shader>
struct ShaderDesc
{
	using ShaderType = Shader;

	std::filesystem::path path;
	std::string entryPoint;
	std::string profile;

	bool operator==(const ShaderDesc& other) const noexcept
	{
		return path == other.path &&
			entryPoint == other.entryPoint &&
			profile == other.profile;
	}
};

inline void hash_combine(std::size_t& seed, const std::size_t value)
{
	seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
}

template <class T>
struct std::hash<ShaderDesc<T>>
{
	std::size_t operator()(const ShaderDesc<T>& desc) const noexcept
	{
		std::size_t h = 0;
		hash_combine(h, std::hash<std::string>{}(desc.path.string()));
		hash_combine(h, std::hash<std::string>{}(desc.entryPoint));
		hash_combine(h, std::hash<std::string>{}(desc.profile));
		return h;
	}
};

#endif
