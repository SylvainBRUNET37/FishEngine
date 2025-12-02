#include "pch.h"
#include "rendering/BillboardRenderer.h"

#include "rendering/graphics/camera/BaseCamera.h"

using namespace DirectX;

BillboardRenderer::BillboardRenderer(ID3D11Device* device)
	: billboardConstantBuffer{device, billboardCbRegisterNumber}
{
}

void BillboardRenderer::Render(Billboard& billboard, ID3D11DeviceContext* context, const XMMATRIX& worldMatrix)
{
	XMStoreFloat4x4(&billboardBuffer.matWorld, XMMatrixTranspose(worldMatrix));

	billboard.shaderProgram.Bind(context);

	billboardConstantBuffer.Update(context, billboardBuffer);
	billboardConstantBuffer.Bind(context);

	context->PSSetShaderResources(0, 1, &billboard.texture.texture);

	Draw(billboard, context);
}

void BillboardRenderer::UpdateCameraData(const BaseCameraData& baseCameraData, ID3D11DeviceContext* context, ID3D11ShaderResourceView* billboardSRV)
{
	XMStoreFloat4x4(&billboardBuffer.matView, XMMatrixTranspose(baseCameraData.matView));
	XMStoreFloat4x4(&billboardBuffer.matProj, XMMatrixTranspose(baseCameraData.matProj));
	XMStoreFloat3(&billboardBuffer.cameraPos, BaseCameraData::position);
}

void BillboardRenderer::Draw(const Billboard& billboard, ID3D11DeviceContext* context)
{
	constexpr UINT stride = sizeof(VertexSprite);
	constexpr UINT offset = 0;

	ID3D11Buffer* vertexBuffer = billboard.vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->Draw(6, 0);
}
