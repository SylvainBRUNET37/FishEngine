#include "pch.h"
#include "rendering/graphics/Mesh.h"

using namespace DirectX;

Mesh::Mesh(std::vector<Vertex>&& vertices_, std::vector<UINT>&& indices_, const UINT materialIndex_,
           ID3D11Device* device)

	: vertices{std::move(vertices_)},
	  indices{std::move(indices_)},
	  materialIndex{materialIndex_},
	  vertexBuffer{device, vertices},
	  indexBuffer{device, indices}
{
}
