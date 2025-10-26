#include "PhysicsEngine/ShapeFactory.h"

#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>

#include "PhysicsEngine/layers/Layers.h"
#include "PhysicsEngine/systems/JoltSystem.h"

using namespace JPH;
using namespace JPH::literals;

Body* ShapeFactory::CreateCube()
{
	const BodyCreationSettings boxSettings
	(
		new BoxShape(Vec3(10.0f, 10.0f, 10.0f)),
		RVec3(5, 50, 0),
		Quat::sIdentity(),
		EMotionType::Dynamic,
		Layers::MOVING
	);

	BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
	Body* body = bodyInterface.CreateBody(boxSettings);
	bodyInterface.AddBody(body->GetID(), EActivation::Activate);

	return body;
}

Body* ShapeFactory::CreatePlane()
{
	const auto planeScale = Vec3(500.0f, 1.0f, 500.0f);
	const Ref planeShape = new BoxShape(planeScale);

	const BodyCreationSettings planeSettings
	(
		planeShape,
		RVec3(0, -5, 0),
		Quat::sIdentity(),
		EMotionType::Static,
		Layers::NON_MOVING
	);

	BodyInterface& bodyInterface = JoltSystem::GetBodyInterface();
	Body* body = bodyInterface.CreateBody(planeSettings);
	bodyInterface.AddBody(body->GetID(), EActivation::Activate);

	return body;
}
