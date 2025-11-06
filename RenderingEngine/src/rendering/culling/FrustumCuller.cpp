#include "pch.h"
#include "rendering/culling/FrustumCuller.h"

bool FrustumCuller::IsMeshCulled(Mesh& mesh, const Transform& transform, BaseCameraData camera) noexcept {

	// Local Mesh bounding box
	DirectX::BoundingBox localBox;
	DirectX::BoundingBox::CreateFromPoints(
		localBox,
		static_cast<UINT>(mesh.vertices.size()),
		&mesh.vertices[0].position,
		sizeof(Vertex)
	);

	// Transform into the world space
	DirectX::BoundingBox worldBox;
	localBox.Transform(worldBox, transform.world);

	// Get the matrices based on the camera data
	auto viewMatrix = XMMatrixLookAtLH(camera.position, camera.focus, camera.up);
	auto projectionMatrix = XMMatrixPerspectiveFovLH(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);

	// Local furstum bounding box
	DirectX::BoundingFrustum frustum;
	DirectX::BoundingFrustum::CreateFromMatrix(frustum, projectionMatrix);

	// Transform into the world space
	DirectX::BoundingFrustum worldFrustum = frustum;
	worldFrustum.Transform(worldFrustum, XMMatrixInverse(nullptr, viewMatrix));

	return !worldFrustum.Intersects(worldBox);
}
