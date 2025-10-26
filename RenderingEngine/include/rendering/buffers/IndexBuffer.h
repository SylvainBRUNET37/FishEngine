#ifndef INDEX_BUFFER
#define INDEX_BUFFER

#include "Buffer.h"
#include <rendering/utils/Util.h>

class IndexBuffer : public Buffer
{
public:
    explicit IndexBuffer(ID3D11Device* device, const std::vector<UINT>& indices)
    {
        D3D11_BUFFER_DESC indexBufferDesc{};

        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(UINT));
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexData{};
        indexData.pSysMem = indices.data();

        DXEssayer(device->CreateBuffer(&indexBufferDesc, &indexData, &buffer));
    }
};

#endif