#ifndef VERTEX_BUFFER
#define VERTEX_BUFFER

#include <vector>

#include "Buffer.h"
#include "rendering/utils/Util.h"

class VertexBuffer : public Buffer
{
public:
    template <typename Vertex>
	explicit VertexBuffer(ID3D11Device* device, const std::vector<Vertex>& vertices)
	{
        D3D11_BUFFER_DESC vertexBufferDesc{};

        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexData{};
        vertexData.pSysMem = vertices.data();

        DXEssayer(device->CreateBuffer(&vertexBufferDesc, &vertexData, &buffer));
	}
};

#endif