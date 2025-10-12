#ifndef SHADER_STAGE_H
#define SHADER_STAGE_H

enum class ShaderStage : uint8_t
{
	Vertex,
	Pixel,
	Geometry,
	Hull,
	Domain,
	Compute
};

#endif