#include "pch.h"
#include "PhysicsEngine/ShapeFactory.h"

#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "PhysicsEngine/layers/Layers.h"
#include "PhysicsEngine/systems/JoltSystem.h"

using namespace JPH;
using namespace DirectX;
using namespace JPH::literals;

Body* ShapeFactory::CreateCubeInVehicleLayer(const Transform& transform)
{
    // Apply scale to the box
    auto halfExtents = Vec3(1.f, 1.f, 1.f);
    halfExtents *= Vec3(transform.scale.x, transform.scale.y, transform.scale.z);

    const RefConst shape = new BoxShape(halfExtents);

    // Convert mesh type of position and rotation to jolt's ones
    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    BodyCreationSettings boxSettings(
        shape,
        position,
        rotation,
        EMotionType::Dynamic,
        Layers::VEHICLE
    );

    boxSettings.mLinearDamping = 1.f;

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(boxSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
    body->SetFriction(0.6f);

    return body;
}

Body* ShapeFactory::CreateCubeInCargoLayer(const Transform& transform)
{
    // Apply scale to the box
    auto halfExtents = Vec3(1.f, 1.f, 1.f);
    halfExtents *= Vec3(transform.scale.x, transform.scale.y, transform.scale.z);

    const RefConst shape = new BoxShape(halfExtents);

    // Convert mesh type of position and rotation to jolt's ones
    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    BodyCreationSettings boxSettings(
        shape,
        position,
        rotation,
        EMotionType::Dynamic,
        Layers::CARGO
    );

    boxSettings.mLinearDamping = 0.5f;

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(boxSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
    bodyInterface.SetRestitution(body->GetID(), 0.5f); //what is restitution supposed to be anyway?
    body->SetFriction(0.6f);

    return body;
}

Body* ShapeFactory::CreateSphere(const Transform& transform, const XMFLOAT3& direction)
{
    // Apply scale to the sphere... but Thierry does not know how to do this or what this multiplication by 0.5 is for...
    /*auto halfExtents = Vec3(0.5f, 0.5f, 0.5f);
    halfExtents *= Vec3(transform.scale.x, transform.scale.y, transform.scale.z);*/

    const RefConst shape = new SphereShape(2.0f * transform.scale.x); //Ah, Thierry comprends peut-être. 1 unité Jolt = 1 metre dans Blender, on dirait...

    // Convert mesh type of position and rotation to jolt's ones
    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    const BodyCreationSettings sphereSettings(
        shape,
        position,
        rotation,
        EMotionType::Dynamic,
        Layers::BALL
    );

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(sphereSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);

    Vec3 velocity(direction.x, direction.y, direction.z);
    velocity = velocity.Normalized();

    static constexpr float shootSpeed = 50.f;
    velocity *= shootSpeed;

    bodyInterface.SetLinearVelocity(body->GetID(), velocity);
    bodyInterface.SetRestitution(body->GetID(), 0.5f);
    body->SetFriction(0.6f);

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

Body* ShapeFactory::CreateCapsule(const Transform& transform)
{
    // TODO: Hardcoded dimension of the capsule in blender 
    const float radius = transform.scale.x * (115/ 2);
    const float halfHeight = transform.scale.y * (215 / 2);

    const RefConst shape = new CapsuleShape(halfHeight, radius);

    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    const BodyCreationSettings capsuleSettings(
        shape,
        position,
        rotation,
        EMotionType::Static,
        Layers::SENSOR
    );

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(capsuleSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
    body->SetIsSensor(true);

    return body;
}
