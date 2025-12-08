#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "layers/Layers.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"

namespace ShapeFactory
{
	[[nodiscard]] JPH::Body* CreateCube(const Transform& transform);
	[[nodiscard]] JPH::Body* CreateCube(const Transform& transform, const Mesh& mesh, JPH::ObjectLayer = Layers::MOVING);
	[[nodiscard]] JPH::Body* CreatePlane(const Transform& transform);
	[[nodiscard]] JPH::Body* CreatePlane(const Transform& transform, const Mesh& mesh);
	[[nodiscard]] JPH::Body* CreateCapsule(const Transform& transform);
	[[nodiscard]] JPH::Body* CreateVerticalCapsule(const Transform& transform, const Mesh& mesh);
	[[nodiscard]] JPH::Body* CreateHorizontalCapsule(const Transform& transform, const Mesh& mesh);
	[[nodiscard]] JPH::Body* CreateSphereWithVelocity(const Transform& transform, const DirectX::XMFLOAT3& direction);
	[[nodiscard]] JPH::Body* CreateFloor();
	[[nodiscard]] JPH::Body* CreateMeshShape(const Transform& transform, const Mesh& mesh);
	[[nodiscard]] JPH::Body* CreateConvexHullShape(const Transform& transform, const Mesh& mesh, bool isDecor = true);
}

#endif