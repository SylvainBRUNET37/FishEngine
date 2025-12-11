#include "pch.h"
#include "PhysicsEngine/ShapeFactory.h"

#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>

#include "PhysicsEngine/layers/Layers.h"
#include "PhysicsEngine/JoltSystem.h"

#include "physicsEngine/utils/MeshUtil.h"
#include "physicsEngine/utils/ConversionUtil.h"

#include "entities/Entity.h"

using namespace JPH;
using namespace DirectX;
using namespace JPH::literals;

Body* ShapeFactory::CreateCube(const Transform& transform, const Entity& entity)
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
        Layers::MOVING
    );

    boxSettings.mLinearDamping = 1.f;
    boxSettings.mUserData = to_uint64(entity);

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(boxSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);

    body->SetFriction(0.6f);

    return body;
}

Body* ShapeFactory::CreateCube(const Transform& transform, const Mesh& mesh, const Entity& entity, JPH::ObjectLayer layer)
{
    // Apply scale to the box
    Vec3 size = MeshUtil::getApproximateSize(mesh);
    auto halfExtents = size * 0.5;
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
        layer
    );

    if (layer != Layers::SINKS)
    {
	    boxSettings.mLinearDamping = 0.9f;
    	boxSettings.mAngularDamping = 0.9f;
        boxSettings.mRestitution = 0.75f;
    }
    else
    {
	    boxSettings.mMaxLinearVelocity = 1'500.0f;
        boxSettings.mRestitution = 0.05f;
    }

    
    boxSettings.mUserData = to_uint64(entity);

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(boxSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);

    body->SetFriction(0.6f);

    return body;
}

Body* ShapeFactory::CreateSphereWithVelocity(const Transform& transform, const XMFLOAT3& direction, const Entity& entity)
{
    // Apply scale to the sphere... but Thierry does not know how to do this or what this multiplication by 0.5 is for...
    /*auto halfExtents = Vec3(0.5f, 0.5f, 0.5f);
    halfExtents *= Vec3(transform.scale.x, transform.scale.y, transform.scale.z);*/

    const RefConst shape = new SphereShape(2.0f * transform.scale.x); //Ah, Thierry comprends peut-être. 1 unité Jolt = 1 metre dans Blender, on dirait...

    // Convert mesh type of position and rotation to jolt's ones
    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    BodyCreationSettings sphereSettings(
        shape,
        position,
        rotation,
        EMotionType::Dynamic,
        Layers::MOVING
    );

    sphereSettings.mUserData = to_uint64(entity);

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(sphereSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);

    Vec3 velocity(direction.x, direction.y, direction.z);
    velocity = velocity.Normalized();

    static constexpr float shootSpeed = 200.f;
    velocity *= shootSpeed;

    bodyInterface.SetLinearVelocity(body->GetID(), velocity);
    bodyInterface.SetRestitution(body->GetID(), 0.5f);
    body->SetFriction(0.6f);

    return body;
}

JPH::Body* ShapeFactory::CreateFloor(const Entity& entity)
{
    const RefConst shape = new PlaneShape(Plane(Vec4Arg(0.0f, 1.0f, 0.0f, 0.0f)));
    const RVec3 position(0.f, 0.f, 0.f);
    const Quat rotation(0.f, 0.f, 0.f, 1.f);


    BodyCreationSettings floorSettings(
        shape,
        position,
        rotation,
        EMotionType::Static,
        Layers::NON_MOVING
    );

    floorSettings.mUserData = to_uint64(entity);

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(floorSettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);

    return body;
}

Body* ShapeFactory::CreatePlane(const Transform& transform, const Entity& entity)
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

Body* ShapeFactory::CreatePlane(const Transform& transform, const Mesh& mesh, const Entity& entity)
{
    // Apply scale to the box
    /*auto size = mesh.getApproximateSize();
    if (size.x == 0)
        size.x += 10;
    if (size.y == 0)
        size.y += 10;
    if (size.z == 0)
        size.z += 10;

    auto halfExtents = Vec3(size.x, size.y, size.z);*/
    auto halfExtents = Vec3(1.f, .5f, 1.f);
    auto size = MeshUtil::getApproximateSize(mesh);
    size *= Vec3(transform.scale.x, transform.scale.y, transform.scale.z);

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

Body* ShapeFactory::CreateCapsule(const Transform& transform, const Entity& entity)
{
    // TODO: Hardcoded dimension of the capsule in blender 
    const float radius = transform.scale.x * (200.0f / 2.0f); //radius
    const float halfHeight = transform.scale.y * (406.0f / 2.0f);

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

Body* ShapeFactory::CreateVerticalCapsule(const Transform& transform, const Mesh& mesh, const Entity& entity)
{
    Vec3 size = MeshUtil::getApproximateSize(mesh);

    //Below assumes a vertically aligned capsule...
    const float radius = transform.scale.x * max(size.GetX(), size.GetZ()) / 2;
    const float halfHeight = transform.scale.y * size.GetY() / 2;

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

Body* ShapeFactory::CreateHorizontalCapsule(const Transform& transform, const Mesh& mesh, const Entity& entity)
{
    Vec3 size = MeshUtil::getApproximateSize(mesh);

    //Below assumes a horizontally aligned capsule...
    const float halfHeight = transform.scale.x * max(size.GetX(), size.GetZ()) / 2;
    const float radius = transform.scale.y * size.GetY() / 2;

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

//Creates a Jolt AABB that perfectly matches the mesh, based on it's triangles (polygons)
JPH::Body* ShapeFactory::CreateMeshShape(const Transform& transform, const Mesh& mesh, const Entity& entity)
{
    const TriangleList triangleList = MeshUtil::generateMeshTriangleList(mesh, MeshUtil::ToJolt(transform.scale));

    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    auto meshSettings = new MeshShapeSettings(triangleList);
    meshSettings->mActiveEdgeCosThresholdAngle = cos(90);

    BodyCreationSettings meshBodySettings(
        meshSettings,
        position,
        rotation,
        EMotionType::Static,
        Layers::NON_MOVING
    );

    meshBodySettings.mUserData = to_uint64(entity);

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(meshBodySettings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);

    return body;
}

Body* ShapeFactory::CreateConvexHullShape(const Transform& transform, const Mesh& mesh, const Entity& entity, const RigidBody::Density density)
{
    Array<Vec3> points;
    points.reserve(mesh.vertices.size());

    const Vec3 scale = ConversionUtil::XMFloat3ToVec3Arg(transform.scale);
    for (const auto& v : mesh.vertices)
        points.push_back(Vec3(v.position.x, v.position.y, v.position.z) * scale);

    // Create hull shape
    const auto* hullSettings = new ConvexHullShapeSettings{ points };
    const ShapeRefC hullShape = hullSettings->Create().Get();

    // Body transform
    const RVec3 position(transform.position.x, transform.position.y, transform.position.z);
    const Quat rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    JPH::ObjectLayer layer = Layers::MOVING;
    if (density != RigidBody::NEUTRAL)
        layer = (density == RigidBody::SINKS) ? Layers::SINKS : Layers::FLOATS;

    BodyCreationSettings settings
	{
        hullShape, 
		position, 
		rotation, 
		EMotionType::Dynamic, 
		layer
	};

    settings.mUserData = to_uint64(entity);

    BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
    Body* body = bodyInterface.CreateBody(settings);
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);

    return body;
}
