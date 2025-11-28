#include "pch.h"
#include "physicsEngine/SensorFactory.h"

#include <Jolt/Physics/Body/BodyInterface.h>

#include "PhysicsEngine/JoltSystem.h"
#include "PhysicsEngine/layers/Layers.h"
#include "physicsEngine/utils/MeshUtil.h"
#include "rendering/core/Transform.h"

using namespace JPH;

Body* SensorFactory::CreateCubeCurrentSensor(const Transform& transform)
{
	const RefConst<Shape> shape = new BoxShape{Vec3{1.f, 1.f, 1.f}};

	// Scale the shape
	if (shape->ScaleShape(MeshUtil::ToJolt(transform.scale)).HasError())
		throw std::runtime_error{"Could not scale the sensor"};

	BodyCreationSettings sensorSettings
	(
		shape,
		MeshUtil::ToJolt(transform.position),
		MeshUtil::ToJolt(transform.rotation),
		EMotionType::Kinematic,
		Layers::SENSOR
	);

	sensorSettings.mIsSensor = true;

	// Create sensor body
	BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
	Body* body = bodyInterface.CreateBody(sensorSettings);

	vassert(body, "An error has occured while creating body (out of body ?)");

	bodyInterface.AddBody(body->GetID(), EActivation::Activate);

	return body;
}
