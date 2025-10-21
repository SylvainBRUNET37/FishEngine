#include "pch.h"
#include "rendering/graphics/Model.h"

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
	: constantBuffer{device, sizeof(ConstantBufferParams)},
	  shaderProgram{std::move(shaderProgram)},
	  meshes{std::move(meshes)},
	  materials{std::move(materials)}
{
}
