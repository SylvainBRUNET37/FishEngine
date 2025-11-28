#include "pch.h"
#include "physicsEngine/SensorFactory.h"

#include <Jolt/Physics/Body/BodyInterface.h>

#include "PhysicsEngine/JoltSystem.h"
#include "PhysicsEngine/layers/Layers.h"
#include "physicsEngine/utils/MeshUtil.h"
#include "rendering/core/Transform.h"

using namespace JPH;

Body* SensorFactory::CreateCubeCurrentSensor(const Transform& transform, const Entity& entity)
{
	auto halfExtents = Vec3(1.f, 1.f, 1.f);
	halfExtents *= Vec3(transform.scale.x, transform.scale.y, transform.scale.z);

	const RefConst shape = new BoxShape{Vec3{1.f, 1.f, 1.f} * halfExtents};

	BodyCreationSettings sensorSettings
	(
		shape,
		MeshUtil::ToJolt(transform.position),
		MeshUtil::ToJolt(transform.rotation),
		EMotionType::Kinematic,
		Layers::SENSOR
	);

	sensorSettings.mIsSensor = true;
	sensorSettings.mUserData = to_uint64(entity);

	// Create sensor body
	BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
	Body* body = bodyInterface.CreateBody(sensorSettings);

	vassert(body, "An error has occured while creating body (out of body ?)");

	bodyInterface.AddBody(body->GetID(), EActivation::Activate);

	return body;
}
