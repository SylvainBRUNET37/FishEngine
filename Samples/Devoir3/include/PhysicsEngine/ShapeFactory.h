#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "rendering/core/Transform.h"

namespace ShapeFactory
{
	[[nodiscard]] JPH::Body* CreateCube(const Transform& transform);
	[[nodiscard]] JPH::Body* CreateCubeInCargoLayer(const Transform& transform);
	[[nodiscard]] JPH::Body* CreatePlane(const Transform& transform);
	[[nodiscard]] JPH::Body* CreateCapsule(const Transform& transform);
	[[nodiscard]] JPH::Body* CreateSphere(const Transform& transform, const DirectX::XMFLOAT3& direction);
}

#endif