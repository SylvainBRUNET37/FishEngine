#include "pch.h"
#include "rendering/graphics/Mesh.h"

void Mesh::Draw(ID3D11DeviceContext* ctx) const
{
    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;

    const auto rawVertexBuffer = vertexBuffer.Get();

    // Bind vertex buffer
    ctx->IASetVertexBuffers(0, 1, &rawVertexBuffer, &stride, &offset);
    ctx->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}
