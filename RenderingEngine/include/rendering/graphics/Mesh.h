#ifndef MESH_H
#define MESH_H

#include <vector>

#include "Vertex.h"
#include "rendering/buffers/IndexBuffer.h"
#include "rendering/buffers/VertexBuffer.h"

struct Mesh
{
	explicit Mesh(std::vector<Vertex>&& vertices_, std::vector<UINT>&& indices_, UINT materialIndex_,
		ID3D11Device* device);

	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	UINT materialIndex;

	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
};

#endif
