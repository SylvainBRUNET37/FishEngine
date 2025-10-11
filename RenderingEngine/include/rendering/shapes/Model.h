#ifndef MODEL_H
#define MODEL_H

#include "Material.h"
#include "Mesh.h"
#include "rendering/ShaderProgram.h"
#include "rendering/device/Device.h"

class Model
{
public:
	explicit Model(std::vector<Mesh>&& meshes, std::vector<Material>&& materials, Device* device, const ShaderProgram& shaderProgram);

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

	Device* device;

	ComPtr<ID3D11Buffer> cbParam;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11InputLayout> inputLayout;

	void Init();
};

#endif
