#ifndef FRUSTUMCULLER_H
#define FRUSTUMCULLER_H

#include <DirectXMath.h>
#include "rendering/graphics/Mesh.h"
#include "rendering/core/Transform.h"

using namespace DirectX;

class FurstumCuller {
	// TODO make singleton
public:

	[[nodiscard]] static bool IsMeshCulled(Mesh& mesh, const Transform& transform) noexcept;

};

#endif
