#ifndef MESH_H
#define MESH_H

#include "Vertex.h"
#include "rendering/graphics/IndexBuffer.h"
#include "rendering/graphics/VertexBuffer.h"

class Mesh
{
public:
	explicit Mesh(std::vector<Vertex>&& vertices_, std::vector<UINT>&& indices_, const UINT materialIndex_, ID3D11Device* device)
		: vertices{std::move(vertices_)},
		  indices{std::move(indices_)},
			materialIndex{ materialIndex_ },
			vertexBuffer{device, vertices},
			indexBuffer{device, indices}
	{
	}

	[[nodiscard]] UINT GetMaterialIndex() const { return materialIndex; }

    void Draw(ID3D11DeviceContext* ctx) const;

private:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

    UINT materialIndex;

	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
};

#endif
