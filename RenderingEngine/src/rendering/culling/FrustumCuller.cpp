#include "pch.h"
#include "rendering/culling/FrustumCuller.h"

#include <DirectXCollision.h>


// Static variables definitions
DirectX::XMMATRIX FrustumCuller::viewMatrixInvert = DirectX::XMMatrixIdentity();
DirectX::XMMATRIX FrustumCuller::projectionMatrix = DirectX::XMMatrixIdentity();
DirectX::BoundingFrustum FrustumCuller::frustum;

void FrustumCuller::Init(const BaseCameraData& camera)
{
	// Get the matrices based on the camera data
	auto viewMatrix = XMMatrixLookAtLH(BaseCameraData::position, camera.focus, camera.up);
	viewMatrixInvert = XMMatrixInverse(nullptr, viewMatrix);
	projectionMatrix = XMMatrixPerspectiveFovLH(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);
	
	// Local furstum bounding box
	frustum = BoundingFrustum(projectionMatrix);
	// Transform into the world space
	frustum.Transform(frustum, viewMatrixInvert);
}

bool FrustumCuller::IsMeshCulled(const Mesh& mesh, const Transform& transform) noexcept
{
	// Transform into the world space
	BoundingBox worldBox;
	mesh.boundingBox.Transform(worldBox, transform.world);

	return !frustum.Intersects(worldBox);
}

bool FrustumCuller::IsBillboardCulled(const Billboard& billboard, const XMMATRIX& worldMatrix) noexcept
{
	// Transform into the world space
	BoundingBox worldBox;
	billboard.boundingBox.Transform(worldBox, worldMatrix);

	return !frustum.Intersects(worldBox);
}
