#include "pch.h"
#include "rendering/shapes/Mesh.h"

void Mesh::Init(ID3D11Device* device)
{
    // Vertex buffer


    // Index buffer
    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(UINT));
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData{};
    indexData.pSysMem = indices.data();
    const HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
    assert(SUCCEEDED(hr));
}

void Mesh::Draw(ID3D11DeviceContext* ctx)
{
    // Bind vertex buffer
    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;

    ctx->IASetVertexBuffers(0, 1, &vertexBuffer.Get(), &stride, &offset);
    ctx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}
