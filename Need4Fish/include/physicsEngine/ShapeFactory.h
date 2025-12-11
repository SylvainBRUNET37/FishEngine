#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "layers/Layers.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"
#include "components/physics/RigidBody.h"

struct Entity;

namespace ShapeFactory
{
	[[nodiscard]] JPH::Body* CreateCube(const Transform& transform, const Entity& entity);
	[[nodiscard]] JPH::Body* CreateCube(const Transform& transform, const Mesh& mesh, const Entity& entity, JPH::ObjectLayer = Layers::MOVING);
	[[nodiscard]] JPH::Body* CreatePlane(const Transform& transform, const Entity& entity);
	[[nodiscard]] JPH::Body* CreatePlane(const Transform& transform, const Mesh& mesh, const Entity& entity);
	[[nodiscard]] JPH::Body* CreateCapsule(const Transform& transform, const Entity& entity);
	[[nodiscard]] JPH::Body* CreateVerticalCapsule(const Transform& transform, const Mesh& mesh, const Entity& entity);
	[[nodiscard]] JPH::Body* CreateHorizontalCapsule(const Transform& transform, const Mesh& mesh, const Entity& entity);
	[[nodiscard]] JPH::Body* CreateSphereWithVelocity(const Transform& transform, const DirectX::XMFLOAT3& direction, const Entity& entity);
	[[nodiscard]] JPH::Body* CreateFloor(const Entity& entity);
	[[nodiscard]] JPH::Body* CreateMeshShape(const Transform& transform, const Mesh& mesh, const Entity& entity);
	[[nodiscard]] JPH::Body* CreateConvexHullShape(const Transform& transform, const Mesh& mesh, const Entity& entity, const RigidBody::Density density = RigidBody::NEUTRAL);
}

#endif