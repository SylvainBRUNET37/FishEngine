#ifndef MESH_H
#define MESH_H

#include "Vertex.h"
#include "rendering/buffers/IndexBuffer.h"
#include "rendering/buffers/VertexBuffer.h"

struct Mesh
{
	explicit Mesh(std::vector<Vertex>&& vertices_, std::vector<UINT>&& indices_, const UINT materialIndex_,
	              ID3D11Device* device)
		: vertices{std::move(vertices_)},
		  indices{std::move(indices_)},
		  materialIndex{materialIndex_},
		  vertexBuffer{device, vertices},
		  indexBuffer{device, indices}
	{
	}

	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	UINT materialIndex;

	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
};

#endif
