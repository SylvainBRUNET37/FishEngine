#include "pch.h"
#include "rendering/graphics/Mesh.h"

using namespace DirectX;

Mesh::Mesh(std::vector<Vertex>&& vertices_, std::vector<UINT>&& indices_, const UINT materialIndex_,
           ID3D11Device* device, const ShaderProgram& shaderProgram_)

	: vertices{std::move(vertices_)},
	  indices{std::move(indices_)},
	  constantBuffer{device, sizeof(ConstantBufferParams)},
	  shaderProgram{shaderProgram_},
	  materialIndex{materialIndex_},
	  vertexBuffer{device, vertices},
	  indexBuffer{device, indices}
{
}
