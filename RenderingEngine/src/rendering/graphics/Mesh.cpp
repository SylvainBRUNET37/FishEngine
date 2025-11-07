#include "pch.h"
#include "rendering/graphics/Mesh.h"
#include <algorithm>

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

//XMFLOAT3 Mesh::getApproximateSize() const
//{
//	float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
//	std::for_each(vertices.begin(), vertices.end(), [&](Vertex vertex) {
//			XMFLOAT3 vPosition = vertex.position;
//			minX = min(minX, vPosition.x);
//			maxX = max(maxX, vPosition.x);
//			minY = min(minY, vPosition.y);
//			maxY = max(maxY, vPosition.y);
//			minZ = min(minZ, vPosition.z);
//			maxZ = max(maxZ, vPosition.z);
//		});
//
//	return XMFLOAT3{ maxX - minX, maxY - minY, maxZ - minZ };
//}

