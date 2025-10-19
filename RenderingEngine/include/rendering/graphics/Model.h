#ifndef MODEL_H
#define MODEL_H

#include "Material.h"
#include "Mesh.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/core/SceneData.h"
#include "rendering/core/Transform.h"
#include "rendering/buffers/ConstantBuffer.h"

class Model
{
public:
	explicit Model(std::vector<Mesh>&& meshes, std::vector<Material>&& materials, ID3D11Device* device,
	               ShaderProgram&& shaderProgram);

	static void Anime(float)
	{
	}

	void Draw(ID3D11DeviceContext* ctx,
	          const Transform& transform,
	          const SceneData& scene);

private:
	struct alignas(16) ConstantBufferParams;

	ConstantBuffer<ConstantBufferParams> constantBuffer;
	ShaderProgram shaderProgram;

	std::vector<Mesh> meshes;
	std::vector<Material> materials;

	static ConstantBufferParams BuildMeshConstantBufferParams(
		const Material& material,
		const Transform& transform,
		const SceneData& scene);
};

#endif
