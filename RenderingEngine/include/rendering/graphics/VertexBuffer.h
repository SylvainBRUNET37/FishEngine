#ifndef VERTEX_BUFFER
#define VERTEX_BUFFER

#include "Buffer.h"
#include "rendering/shapes/Vertex.h"

class VertexBuffer : public Buffer
{
public:
	explicit VertexBuffer(ID3D11Device* device, const std::vector<Vertex>& vertices)
	{
        D3D11_BUFFER_DESC vertexBufferDesc{};

        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexData{};
        vertexData.pSysMem = vertices.data();

        const HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &buffer);
        assert(SUCCEEDED(hr));
	}
};

#endif