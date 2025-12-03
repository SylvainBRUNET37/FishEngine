#ifndef FRUSTUMCULLER_H
#define FRUSTUMCULLER_H

#include <DirectXMath.h>
#include "rendering/graphics/Mesh.h"
#include "rendering/graphics/Billboard.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/camera/baseCamera.h"

using namespace DirectX;

class FrustumCuller {
public:

	static void Init(const BaseCameraData& camera);
 	[[nodiscard]] static bool IsMeshCulled(const Mesh& mesh, const Transform& transform) noexcept;
	[[nodiscard]] static bool IsBillboardCulled(const Billboard& billboard, const XMMATRIX& worldMatrix) noexcept;

private:
	static XMMATRIX viewMatrixInvert;
	static XMMATRIX projectionMatrix;
	static BoundingFrustum frustum;
};

#endif
