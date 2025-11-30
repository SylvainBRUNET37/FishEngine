#include "pch.h"
#include "rendering/BillboardRenderer.h"

#include "rendering/graphics/camera/BaseCamera.h"

using namespace DirectX;

BillboardRenderer::BillboardRenderer(ID3D11Device* device)
	: billboardConstantBuffer{device, billboardCbRegisterNumber}
{
}

void BillboardRenderer::Render(Billboard& billboard, ID3D11DeviceContext* context, const BaseCameraData& baseCameraData)
{
	BillboardBuffer billboardBuffer{};

	XMStoreFloat4x4(&billboardBuffer.matWorld, XMMatrixTranspose(ComputeBillboardWorldMatrix(billboard)));
	XMStoreFloat4x4(&billboardBuffer.matView, XMMatrixTranspose(baseCameraData.matView));
	XMStoreFloat4x4(&billboardBuffer.matProj, XMMatrixTranspose(baseCameraData.matProj));
	XMStoreFloat3(&billboardBuffer.cameraPos, BaseCameraData::position);

	billboard.shaderProgram.Bind(context);

	billboardConstantBuffer.Update(context, billboardBuffer);
	billboardConstantBuffer.Bind(context);

	context->PSSetShaderResources(0, 1, &billboard.texture.texture);

	Draw(billboard, context);
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

XMMATRIX BillboardRenderer::ComputeBillboardWorldMatrix(const Billboard& billboard)
{
	// Compute the direction from billboard to camera
	const XMVECTOR billboardPosition = XMLoadFloat3(&billboard.position);
	XMVECTOR directionToCamera = BaseCameraData::position - billboardPosition;

	// If cylindrical, ignore vertical
	if (billboard.type == Billboard::Cylindric)
	{
		directionToCamera = XMVectorSetY(directionToCamera, 0.0f);

		// Avoid zero length vector if camera is exactly above or below the billboard
		if (XMVector3Equal(directionToCamera, XMVectorZero()))
			directionToCamera = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}

	static constexpr auto WORLD_UP = XMVECTOR{0, 1, 0, 0};

	const XMVECTOR billboardForward = XMVector3Normalize(directionToCamera);
	const XMVECTOR billboardRight = XMVector3Normalize(XMVector3Cross(WORLD_UP, billboardForward));
	const XMVECTOR billboardUp = XMVector3Cross(billboardForward, billboardRight);

	// Define the billboard orientation with right, up and forward
	const XMMATRIX billboardRotation =
	{
		billboardRight,
		billboardUp,
		-billboardForward, // minus because we are right handed
		XMVectorSet(0, 0, 0, 1)
	};

	// Build billboard world matrix (S * R * T)
	const XMMATRIX billboardMatWorld =
		XMMatrixScaling(billboard.scale.x, billboard.scale.y, 1.0f) *
		billboardRotation *
		XMMatrixTranslation
		(
			billboard.position.x,
			billboard.position.y,
			billboard.position.z
		);

	return billboardMatWorld;
}
