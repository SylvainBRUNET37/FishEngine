#ifndef FRUSTUMCULLER_H
#define FRUSTUMCULLER_H

#include <DirectXMath.h>
#include "rendering/graphics/Mesh.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/camera/baseCamera.h"

using namespace DirectX;

class FrustumCuller {
public:

	[[nodiscard]] static bool IsMeshCulled(const Mesh& mesh, const Transform& transform, const BaseCameraData& camera) noexcept;

};

#endif
