#include "pch.h"
#include "rendering/Renderer.h"

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

void Renderer::Draw(Model& model, ID3D11DeviceContext* context, const Transform& transform, const SceneData& scene)
{
	model.shaderProgram.Bind(context);

	// Prepare constant buffer once for each mesh
	for (size_t i = 0; i < model.meshes.size(); ++i)
	{
		auto& mat = model.materials[model.meshes[i].GetMaterialIndex()];

		Model::ConstantBufferParams params = BuildMeshConstantBufferParams(mat, transform, scene);

		model.constantBuffer.Update(context, params);
		model.constantBuffer.Bind(context);

		mat.Bind(context);

		model.meshes[i].Draw(context);
	}
}

Model::ConstantBufferParams Renderer::BuildMeshConstantBufferParams(const Material& material,
	const Transform& transform, const SceneData& scene)
{
	Model::ConstantBufferParams params;

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
