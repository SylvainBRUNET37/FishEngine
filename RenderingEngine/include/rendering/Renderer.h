#ifndef RENDERER_H
#define RENDERER_H

#include "core/SceneData.h"
#include "core/Transform.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"

class Renderer
{
public:
	explicit Renderer(ID3D11Device* device, std::vector<Material>&& materials, const int frameCbRegisterNumber,
	                  const int objectCbRegisterNumber)
		: frameConstantBuffer{device, frameCbRegisterNumber}, objectConstantBuffer{device, objectCbRegisterNumber},
		  materials{std::move(materials)}
	{
	}

	void Render(const Mesh& mesh, ID3D11DeviceContext* context,
	          const Transform& transform,
	          const SceneData& scene);

private:
	struct FrameBufferData // b0 in the shader program
	{
		DirectX::XMMATRIX matViewProj;
		DirectX::XMFLOAT4 vLumiere;
		DirectX::XMFLOAT4 vCamera;
		DirectX::XMFLOAT4 vAEcl;
		DirectX::XMFLOAT4 vDEcl;
		DirectX::XMFLOAT4 vSEcl;
	};

	struct ObjectConstants // b1 in the shader program
	{
		DirectX::XMMATRIX matWorld;
	};

	ConstantBuffer<FrameBufferData> frameConstantBuffer;
	ConstantBuffer<ObjectConstants> objectConstantBuffer;
	std::vector<Material> materials;

	static void Draw(const Mesh& mesh, ID3D11DeviceContext* context);

	static FrameBufferData BuildFrameConstantBufferParams(const SceneData& sceneData);
	static ObjectConstants BuildObjectConstantBufferParams(const Transform& transform);
	static Material::MaterialBufferData BuildMaterialConstantBufferParams(const Material& material);
};

#endif
