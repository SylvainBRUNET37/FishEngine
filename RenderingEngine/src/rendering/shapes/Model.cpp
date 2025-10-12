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
                 const XMMATRIX& world,
                 const XMMATRIX& view,
                 const XMMATRIX& proj,
                 const XMFLOAT4& lightPos,
                 const XMFLOAT4& cameraPos,
                 const XMFLOAT4& vAEcl,
                 const XMFLOAT4& vDEcl,
                 const XMFLOAT4& vSEcl)
{
	shaderProgram.Bind(ctx);

	// Prepare CB once for each mesh (here per-mesh since material differs)
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto& mat = materials[meshes[i].GetMaterialIndex()];

		ConstantBufferParams params = BuildPerMeshParams(
			mat, world, view, proj, lightPos, cameraPos, vAEcl, vDEcl, vSEcl);

		constantBuffer.Update(ctx, params);
		constantBuffer.Bind(ctx);

		mat.Bind(ctx);

		meshes[i].Draw(ctx);
	}
}

auto Model::BuildPerMeshParams(
	const Material& mat,
	const XMMATRIX& world,
	const XMMATRIX& view,
	const XMMATRIX& proj,
	const XMFLOAT4& lightPos,
	const XMFLOAT4& cameraPos,
	const XMFLOAT4& vAEcl,
	const XMFLOAT4& vDEcl,
	const XMFLOAT4& vSEcl) -> ConstantBufferParams
{
	ConstantBufferParams params;

	params.matWorld = XMMatrixTranspose(world);
	params.matWorldViewProj = XMMatrixTranspose(world * view * proj);
	params.vLumiere = lightPos;
	params.vCamera = cameraPos;
	params.vAEcl = vAEcl;
	params.vDEcl = vDEcl;
	params.vSEcl = vSEcl;
	params.vAMat = mat.ambient;
	params.vDMat = mat.diffuse;
	params.vSMat = mat.specular;
	params.puissance = mat.shininess;
	params.bTex = mat.texture != nullptr;
	params.remplissage = XMFLOAT2(0, 0);

	return params;
}
