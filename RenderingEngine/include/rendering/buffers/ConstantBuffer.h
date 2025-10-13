#ifndef CONSTANT_BUFFER
#define CONSTANT_BUFFER

#include "Buffer.h"

template<class Param>
class ConstantBuffer : public Buffer
{
public:
    explicit ConstantBuffer(ID3D11Device* device)
    {
        D3D11_BUFFER_DESC constantBufferData{};

        constantBufferData.Usage = D3D11_USAGE_DYNAMIC;
        constantBufferData.ByteWidth = sizeof(Param);
        constantBufferData.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferData.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        const HRESULT hr = device->CreateBuffer(&constantBufferData, nullptr, &buffer);
        assert(SUCCEEDED(hr));
    }

    void Update(ID3D11DeviceContext* context, const Param& data)
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        const HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

        assert(SUCCEEDED(hr));
        std::memcpy(mapped.pData, &data, sizeof(Param));

        context->Unmap(buffer, 0);
    }

    void Bind(ID3D11DeviceContext* context)
    {
        context->VSSetConstantBuffers(0, 1, &buffer);
        context->PSSetConstantBuffers(0, 1, &buffer);
    }
};

#endif