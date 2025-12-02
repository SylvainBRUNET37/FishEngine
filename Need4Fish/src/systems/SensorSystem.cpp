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

		const auto sensorEntity = to_entity(bodyInterface.GetUserData(sensorId));
		const auto& sensor = entityManager.Get<Sensor>(sensorEntity);

		static constexpr float SPEED_LIMIT = 400.0f;
		const bool shouldApplyForce = bodyInterface.GetMotionType(objectId) == EMotionType::Dynamic &&
			bodyInterface.GetLinearVelocity(objectId).Length() < SPEED_LIMIT;
		if (shouldApplyForce)
		{
			static constexpr float WORLD_SIZE_FACTOR = 30000000;
			const Vec3 impulse = sensor.direction * sensor.pushStrength;
			
			bodyInterface.AddForce(objectId, impulse * WORLD_SIZE_FACTOR);
		}
	}
}
