#ifndef MESH_H
#define MESH_H

#include "Vertex.h"
#include "rendering/device/GraphicsDevice.h"
#include "rendering/graphics/VertexBuffer.h"

class Mesh
{
public:
	explicit Mesh(std::vector<Vertex>&& vertices_, std::vector<UINT>&& indices_, const UINT materialIndex_, ID3D11Device* device)
		: vertices{std::move(vertices_)},
		  indices{std::move(indices_)},
			materialIndex{ materialIndex_ },
			vertexBuffer{device, vertices}
	{
	}

	[[nodiscard]] UINT GetMaterialIndex() const { return materialIndex; }

	void Init(ID3D11Device* device);
    void Draw(ID3D11DeviceContext* ctx);

private:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

    UINT materialIndex;

	VertexBuffer vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
};

#endif
