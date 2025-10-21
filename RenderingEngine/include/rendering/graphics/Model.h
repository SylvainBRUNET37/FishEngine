#ifndef MODEL_H
#define MODEL_H

#include "Material.h"
#include "Mesh.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/buffers/ConstantBuffer.h"

struct Model
{
	explicit Model(std::vector<Mesh>&& meshes, std::vector<Material>&& materials, ID3D11Device* device,
	               ShaderProgram&& shaderProgram);

	struct alignas(16) ConstantBufferParams;

	ConstantBuffer constantBuffer;
	ShaderProgram shaderProgram;

	std::vector<Mesh> meshes;
	std::vector<Material> materials;
};

#endif
