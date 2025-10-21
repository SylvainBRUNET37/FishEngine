#ifndef MESH_H
#define MESH_H

#include "Vertex.h"
#include "rendering/buffers/ConstantBuffer.h"
#include "rendering/buffers/IndexBuffer.h"
#include "rendering/buffers/VertexBuffer.h"
#include "rendering/shaders/ShaderProgram.h"

struct Mesh
{
	struct alignas(16) ConstantBufferParams
	{
		DirectX::XMMATRIX matWorldViewProj;
		DirectX::XMMATRIX matWorld;
		DirectX::XMFLOAT4 vLumiere;
		DirectX::XMFLOAT4 vCamera;
		DirectX::XMFLOAT4 vAEcl;
		DirectX::XMFLOAT4 vAMat;
		DirectX::XMFLOAT4 vDEcl;
		DirectX::XMFLOAT4 vDMat;
		DirectX::XMFLOAT4 vSEcl;
		DirectX::XMFLOAT4 vSMat;
		float puissance;
		int bTex;
		DirectX::XMFLOAT2 remplissage;
	};

	explicit Mesh(std::vector<Vertex>&& vertices_, std::vector<UINT>&& indices_, const UINT materialIndex_,
		ID3D11Device* device, const ShaderProgram& shaderProgram_);

	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	ConstantBuffer constantBuffer;
	ShaderProgram shaderProgram;

	UINT materialIndex;

	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
};

#endif
