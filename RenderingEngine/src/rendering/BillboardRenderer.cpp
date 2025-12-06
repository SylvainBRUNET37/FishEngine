#include "pch.h"
#include "rendering/BillboardRenderer.h"

#include "rendering/graphics/camera/BaseCamera.h"

using namespace DirectX;
using namespace std;

BillboardRenderer::BillboardRenderer(ID3D11Device* device)
	: billboardCameraConstantBuffer{device, billboardCameraCbRegisterNumber},
	  billboardConstantBuffer{device, billboardCbRegisterNumber}
{
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(BillboardData) * MAX_BILLBOARDS;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(BillboardData);

	DXEssayer(device->CreateBuffer(&desc, nullptr, &billboardWorldBuffer));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = MAX_BILLBOARDS;

	DXEssayer(device->CreateShaderResourceView(billboardWorldBuffer, &srvDesc, &billboardWorldSRV));
}

void BillboardRenderer::Render(Billboard& billboard, ID3D11DeviceContext* context, const XMMATRIX& worldMatrix)
{
	XMStoreFloat4x4(&billboardBuffer.matWorld, XMMatrixTranspose(worldMatrix));

	billboard.shaderProgram->Bind(context);

	// TODO: do it only once per frame
	billboardCameraConstantBuffer.Update(context, billboardCameraBuffer);
	billboardCameraConstantBuffer.Bind(context);

	billboardConstantBuffer.Update(context, billboardBuffer);
	billboardConstantBuffer.Bind(context);

	context->PSSetShaderResources(0, 1, &billboard.texture.texture);

	Draw(billboard, context);
}

void BillboardRenderer::RenderWithInstancing(Billboard& billboard, ID3D11DeviceContext* context,
	const vector<BillboardData>& worldMatrices)
{
	context->UpdateSubresource(billboardWorldBuffer, 0, nullptr, worldMatrices.data(), 0, 0);
	billboard.shaderProgram->Bind(context);

	billboardCameraConstantBuffer.Update(context, billboardCameraBuffer);
	billboardCameraConstantBuffer.Bind(context);

	// Bind the vertex buffer
	constexpr UINT stride = sizeof(VertexSprite);
	constexpr UINT offset = 0;
	ID3D11Buffer* billboardVB = billboard.vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &billboardVB, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind SRV of the structured buffer
	context->VSSetShaderResources(1, 1, &billboardWorldSRV);
	context->PSSetShaderResources(0, 1, &billboard.texture.texture);

	// Draw every billboards
	context->DrawInstanced(6, static_cast<UINT>(worldMatrices.size()), 0, 0);
}

void BillboardRenderer::UpdateCameraData(const BaseCameraData& baseCameraData, ID3D11DeviceContext* context, ID3D11ShaderResourceView* billboardSRV)
{
	XMStoreFloat4x4(&billboardCameraBuffer.matView, XMMatrixTranspose(baseCameraData.matView));
	XMStoreFloat4x4(&billboardCameraBuffer.matProj, XMMatrixTranspose(baseCameraData.matProj));
	XMStoreFloat3(&billboardCameraBuffer.cameraPos, BaseCameraData::position);
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
