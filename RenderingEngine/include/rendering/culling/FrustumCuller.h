#ifndef FRUSTUMCULLER_H
#define FRUSTUMCULLER_H

#include <DirectXMath.h>
#include "rendering/graphics/Mesh.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/camera/baseCamera.h"

using namespace DirectX;

class FrustumCuller {
public:

	[[nodiscard]] static bool IsMeshCulled(Mesh& mesh, const Transform& transform, BaseCameraData camera) noexcept;

};

#endif
