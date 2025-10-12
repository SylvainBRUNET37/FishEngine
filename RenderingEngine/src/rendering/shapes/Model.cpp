#include "pch.h"
#include "rendering/shapes/Model.h"

using namespace DirectX;

struct alignas(16) Model::ConstantBufferParams
{
	XMMATRIX matWorldViewProj;
	XMMATRIX matWorld;
	XMFLOAT4 vLumiere;
	XMFLOAT4 vCamera;
	XMFLOAT4 vAEcl;
	XMFLOAT4 vAMat;
	XMFLOAT4 vDEcl;
	XMFLOAT4 vDMat;
	XMFLOAT4 vSEcl;
	XMFLOAT4 vSMat;
	float puissance;
	int bTex;
	XMFLOAT2 remplissage;
};


Model::Model(std::vector<Mesh>&& meshes, std::vector<Material>&& materials, ID3D11Device* device,
             ShaderProgram&& shaderProgram)
	: constantBuffer{device},
	  shaderProgram{std::move(shaderProgram)},
	  meshes{std::move(meshes)},
	  materials{std::move(materials)}
{
}

void Model::Draw(ID3D11DeviceContext* ctx,
                 const Transform& transform,
                 const SceneData& scene)
{
	shaderProgram.Bind(ctx);

	// Prepare CB once for each mesh (here per-mesh since material differs)
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto& mat = materials[meshes[i].GetMaterialIndex()];

		ConstantBufferParams params = BuildPerMeshParams(
			mat, transform, scene);

		constantBuffer.Update(ctx, params);
		constantBuffer.Bind(ctx);

		mat.Bind(ctx);

		meshes[i].Draw(ctx);
	}
}

Model::ConstantBufferParams Model::BuildPerMeshParams(
	const Material& material,
	const Transform& transform,
	const SceneData& scene)
{
	ConstantBufferParams params;

	params.matWorld = XMMatrixTranspose(transform.world);
	params.matWorldViewProj = XMMatrixTranspose(transform.world * transform.view * transform.proj);
	params.vLumiere = scene.lightPosition;
	params.vCamera = scene.cameraPosition;
	params.vAEcl = scene.vAEcl;
	params.vDEcl = scene.vDEcl;
	params.vSEcl = scene.vSEcl;
	params.vAMat = material.ambient;
	params.vDMat = material.diffuse;
	params.vSMat = material.specular;
	params.puissance = material.shininess;
	params.bTex = (material.texture != nullptr);
	params.remplissage = XMFLOAT2(0, 0);

	return params;
}
