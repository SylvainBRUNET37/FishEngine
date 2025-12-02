#include "pch.h"
#include "rendering/culling/FrustumCuller.h"

#include <DirectXCollision.h>

bool FrustumCuller::IsMeshCulled(const Mesh& mesh, const Transform& transform, const BaseCameraData& camera) noexcept
{
	// Transform into the world space
	BoundingBox worldBox;
	mesh.boundingBox.Transform(worldBox, transform.world);

	// Get the matrices based on the camera data
	const auto viewMatrix = XMMatrixLookAtLH(BaseCameraData::position, camera.focus, camera.up);
	const auto projectionMatrix = XMMatrixPerspectiveFovLH(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);

	// Local furstum bounding box
	BoundingFrustum frustum{ projectionMatrix };

	// Transform into the world space
	frustum.Transform(frustum, XMMatrixInverse(nullptr, viewMatrix));

	return !frustum.Intersects(worldBox);
}

bool FrustumCuller::IsBillboardCulled(const Billboard& billboard, const XMMATRIX& worldMatrix, const BaseCameraData& camera) noexcept
{
	// Transform into the world space
	BoundingBox worldBox;
	billboard.boundingBox.Transform(worldBox, worldMatrix);

	// Get the matrices based on the camera data
	const auto viewMatrix = XMMatrixLookAtLH(BaseCameraData::position, camera.focus, camera.up);
	const auto projectionMatrix = XMMatrixPerspectiveFovLH(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);

	// Local furstum bounding box
	BoundingFrustum frustum{ projectionMatrix };

	// Transform into the world space
	frustum.Transform(frustum, XMMatrixInverse(nullptr, viewMatrix));

	return !frustum.Intersects(worldBox);
}
