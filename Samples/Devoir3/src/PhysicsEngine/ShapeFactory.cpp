#include "pch.h"
#include "PhysicsEngine/ShapeFactory.h"

#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

#include "PhysicsEngine/layers/Layers.h"
#include "PhysicsEngine/systems/JoltSystem.h"

using namespace JPH;
using namespace JPH::literals;

Body* ShapeFactory::CreateCube(const Transform& transform)
{
    // Apply scale to the box
    auto halfExtents = Vec3(1.f, 1.f, 1.f);
    halfExtents *= Vec3(transform.scale.x, transform.scale.y, transform.scale.z);

    const RefConst shape = new BoxShape(halfExtents);

    // Convert mesh type of position and rotation to jolt's ones
    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    const BodyCreationSettings boxSettings(
        shape,
        position,
        rotation,
        EMotionType::Dynamic,
        Layers::MOVING
    );

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(boxSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);

    return body;
}

Body* ShapeFactory::CreatePlane(const Transform& transform)
{
    // Apply scale to the box
	auto halfExtents = Vec3(1.f, 0.05f, 1.f);
    halfExtents *= Vec3(transform.scale.x, transform.scale.y, transform.scale.z);

	const RefConst shape = new BoxShape(halfExtents);

    // Convert mesh type of position and rotation to jolt's ones
    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    const BodyCreationSettings planeSettings(
        shape,
        position,
        rotation,
        EMotionType::Static,
        Layers::NON_MOVING
    );

	BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
	Body* body = bodyInterface.CreateBody(planeSettings);
	bodyInterface.AddBody(body->GetID(), EActivation::Activate);

	return body;
}

Body* ShapeFactory::CreateCylinder(const Transform& transform)
{
    const float radius = transform.scale.x * 1.0f;
    const float halfHeight = transform.scale.y * 1.0f;

    const RefConst shape = new CylinderShape(halfHeight, radius);

    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    const BodyCreationSettings cylinderSettings(
        shape,
        position,
        rotation,
        EMotionType::Static,
        Layers::NON_MOVING
    );

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(cylinderSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);

    return body;
}