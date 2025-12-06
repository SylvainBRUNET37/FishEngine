#include "pch.h"
#include "rendering/graphics/Billboard.h"
#include <rendering/graphics/camera/BaseCamera.h>

using namespace DirectX;

Billboard::Billboard(const std::shared_ptr<ShaderProgram>& shaderProgram_, const Texture& texture_,
                     ID3D11Device* device,
                     const XMFLOAT3 position, const XMFLOAT2 scale, const Type type)
	: position{position},
	  scale{scale},
	  vertices{ComputeVertices()},
	  texture{texture_},
	  shaderProgram{shaderProgram_},
	  vertexBuffer{device, vertices},
	  type{type}
{
	BoundingBox::CreateFromPoints
	(
		boundingBox,
		vertices.size(),
		&vertices[0].position,
		sizeof(VertexSprite)
	);
}

std::vector<VertexSprite> Billboard::ComputeVertices()
{
	return
	{
		{{-0.5f, -0.5f, 0.0f}, {0, 1}}, // bottom-left
		{{0.5f, -0.5f, 0.0f}, {1, 1}}, // bottom-right
		{{-0.5f, 0.5f, 0.0f}, {0, 0}}, // top-left

		{{0.5f, -0.5f, 0.0f}, {1, 1}}, // bottom-right
		{{0.5f, 0.5f, 0.0f}, {1, 0}}, // top-right
		{{-0.5f, 0.5f, 0.0f}, {0, 0}} // top-left
	};
}

XMMATRIX Billboard::ComputeCylindricBillboardWorldMatrix() const
{
	// Compute the direction from billboard to camera
	const XMVECTOR billboardPosition = XMLoadFloat3(&position);
	XMVECTOR directionToCamera = BaseCameraData::position - billboardPosition;

	// Ignore vertical
	directionToCamera = XMVectorSetY(directionToCamera, 0.0f);

	// Avoid zero length vector if camera is exactly above or below the billboard
	if (XMVector3Equal(directionToCamera, XMVectorZero()))
		directionToCamera = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	return ComputeBillboardWorldMatrix(directionToCamera);
}

XMMATRIX Billboard::ComputeCameraFacingBillboardWorldMatrix() const
{
	// Compute the direction from billboard to camera
	const XMVECTOR billboardPosition = XMLoadFloat3(&position);
	const XMVECTOR directionToCamera = BaseCameraData::position - billboardPosition;

	return ComputeBillboardWorldMatrix(directionToCamera);
}

XMMATRIX Billboard::ComputeBillboardWorldMatrix(const XMVECTOR& directionToCamera) const
{
	static constexpr auto WORLD_UP = XMVECTOR{0, 1, 0, 0};

	const XMVECTOR billboardForward = XMVector3Normalize(directionToCamera);
	const XMVECTOR billboardRight = XMVector3Cross(WORLD_UP, billboardForward);
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
	return
	{
		XMMatrixScaling(scale.x, scale.y, 1.0f) *
		billboardRotation *
		XMMatrixTranslation
		(
			position.x,
			position.y,
			position.z
		)
	};
}
