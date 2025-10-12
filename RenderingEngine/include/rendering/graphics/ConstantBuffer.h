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
};

#endif