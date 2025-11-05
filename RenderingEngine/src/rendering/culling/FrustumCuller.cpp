#include "rendering/culling/FrustumCuller.h"

bool FurstumCuller::IsMeshCulled(Mesh& mesh, const Transform& transform) noexcept {

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

	// Local furstum bounding box
	DirectX::BoundingFrustum frustum;
	// TODO: get projection matrix
	//DirectX::BoundingFrustum::CreateFromMatrix(frustum, projectionMatrix);

	// Transform into the world space
	DirectX::BoundingFrustum worldFrustum = frustum;
	// TODO: get view matrix
	//worldFrustum.Transform(worldFrustum, XMMatrixInverse(nullptr, viewMatrix));



	// worldFrustum.Intersects(worldBox)
	return false;
}