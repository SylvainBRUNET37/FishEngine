#ifndef MODEL_H
#define MODEL_H

#include "Material.h"
#include "Mesh.h"
#include "rendering/ShaderProgram.h"
#include "rendering/device/GraphicsDevice.h"
#include "rendering/graphics/ConstantBuffer.h"

class Model
{
public:
	explicit Model(std::vector<Mesh>&& meshes, std::vector<Material>&& materials, GraphicsDevice* device, ShaderProgram&& shaderProgram);

	static void Anime(float)
	{
	}

	void Draw(ID3D11DeviceContext* ctx,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj,
		const DirectX::XMFLOAT4& lightPos,
		const DirectX::XMFLOAT4& cameraPos,
		const DirectX::XMFLOAT4& vAEcl,
		const DirectX::XMFLOAT4& vDEcl,
		const DirectX::XMFLOAT4& vSEcl);

private:
	std::vector<Mesh> meshes;
	std::vector<Material> materials;

	DirectX::XMMATRIX matWorld;

	GraphicsDevice* device;

	struct alignas(16) ConstantBufferParams;
	ConstantBuffer<ConstantBufferParams> constantBuffer;

	ShaderProgram shaderProgram;
};

#endif
