#include "pch.h"
#include "systems/SensorSystem.h"

#include "physicsEngine/JoltSystem.h"

using namespace std;
using namespace JPH;

void SensorSystem::RemoveContact(const std::pair<BodyID, BodyID>& contact)
{
	erase_if(activeContacts, [&](const auto& activeContact)
	{
		return activeContact.first == contact.first
			&& activeContact.second == contact.second
			|| activeContact.first == contact.second
			&& activeContact.second == contact.first;
	});
}

void SensorSystem::Update(double deltaTime, EntityManager& entityManager)
{
	// Iterate on object which are in the sensor
	for (const auto& [sensorId, objectId] : activeContacts)
	{
		auto& bodyInterface = JoltSystem::GetBodyInterface();
		vassert(bodyInterface.IsSensor(sensorId), "The key of activeContacts must be the sensor");

		if (bodyInterface.GetMotionType(objectId) == EMotionType::Dynamic)
		{
			const Vec3 objectForward = bodyInterface.GetRotation(objectId).RotateAxisZ();

			constexpr float pushStrength = 50.0f;
			const Vec3 impulse = objectForward * pushStrength;

			bodyInterface.AddLinearVelocity(objectId, impulse);
		}
	}
}
