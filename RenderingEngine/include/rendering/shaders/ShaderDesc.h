#ifndef SHADER_DESC_H
#define SHADER_DESC_H

#include <filesystem>

enum class ShaderStage : uint8_t
{
	Vertex,
	Pixel,
	Geometry,
	Hull,
	Domain,
	Compute
};

struct ShaderDesc
{
	std::filesystem::path path;
	std::string entryPoint;
	std::string profile;
	ShaderStage stage;

	bool operator==(const ShaderDesc& other) const noexcept
	{
		return path == other.path &&
			entryPoint == other.entryPoint &&
			profile == other.profile &&
			stage == other.stage;
	}
};

inline void hash_combine(std::size_t& seed, const std::size_t value)
{
	seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
}

template <>
struct std::hash<ShaderDesc>
{
	std::size_t operator()(const ShaderDesc& desc) const noexcept
	{
		std::size_t h = 0;

		hash_combine(h, std::hash<std::string>{}(desc.path.string()));
		hash_combine(h, std::hash<std::string>{}(desc.entryPoint));
		hash_combine(h, std::hash<std::string>{}(desc.profile));
		hash_combine(h, std::hash<int>{}(static_cast<int>(desc.stage)));

		return h;
	}
};

#endif
