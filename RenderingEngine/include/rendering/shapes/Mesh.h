#ifndef MESH_H
#define MESH_H

#include "Vertex.h"
#include "rendering/device/Device.h"

class Mesh
{
public:
	explicit Mesh(std::vector<Vertex>&& vertices, std::vector<UINT>&& indices, UINT materialIndex_)
		: vertices{std::move(vertices)},
		  indices{std::move(indices)},
			materialIndex{ materialIndex_ }
	{
	}

    UINT GetMaterialIndex() const { return materialIndex; }

    void Initialize(ID3D11Device* device)
    {
        // Vertex buffer
        D3D11_BUFFER_DESC vbd = {};
        vbd.Usage = D3D11_USAGE_DEFAULT;
        vbd.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vinit = {};
        vinit.pSysMem = vertices.data();
        HRESULT hr = device->CreateBuffer(&vbd, &vinit, &vertexBuffer);
        assert(SUCCEEDED(hr));

        // Index buffer
        D3D11_BUFFER_DESC ibd = {};
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.ByteWidth = static_cast<UINT>(indices.size() * sizeof(UINT));
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA iinit = {};
        iinit.pSysMem = indices.data();
        hr = device->CreateBuffer(&ibd, &iinit, &indexBuffer);
        assert(SUCCEEDED(hr));
    }

    void Draw(ID3D11DeviceContext* ctx)
    {
        // Bind vertex buffer
        constexpr UINT stride = sizeof(Vertex);
        constexpr UINT offset = 0;

        ctx->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        ctx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ctx->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
    }

private:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

    UINT materialIndex;

	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
};

#endif
