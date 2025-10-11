#ifndef MESH_H
#define MESH_H

#include "Objet3D.h"
#include "Vertex.h"
#include "rendering/utils/ComPtr.h"

class Mesh : public CObjet3D
{
public:
	explicit Mesh(std::vector<Vertex>&& vertices, std::vector<UINT>&& indices)
		: vertices{std::move(vertices)},
		  indices{std::move(indices)}
	{
	}

	void Anime(const float tempEcoule) override
	{
	}

	void Draw(const DirectX::XMMATRIX& viewProj) override
	{
	}

private:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	ComPtr<ID3D11SamplerState> pSampleState;
	ComPtr<ID3D11Buffer> pConstantBuffer;
	//ID3DX11Effect* pEffet;
	//ID3DX11EffectTechnique* pTechnique;
	//ID3DX11EffectPass* pPasse;
	ComPtr<ID3D11InputLayout> pVertexLayout;
};

#endif
