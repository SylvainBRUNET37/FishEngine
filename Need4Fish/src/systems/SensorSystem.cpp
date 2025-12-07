#include "pch.h"
#include "systems/SensorSystem.h"

#include "physicsEngine/JoltSystem.h"

using namespace std;
using namespace JPH;

void SensorSystem::AddContact(pair<BodyID, BodyID> contact)
{
	activeContacts.emplace_back(contact);
}

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

void SensorSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	// Iterate on object which are in the sensor
	for (const auto& [sensorId, objectId] : activeContacts)
	{
		static auto& bodyInterface = JoltSystem::GetBodyInterface();
		vassert(bodyInterface.IsSensor(sensorId), "The key of activeContacts must be the sensor");

		const auto sensorEntity = to_entity(bodyInterface.GetUserData(sensorId));
		const auto& sensor = entityManager.Get<Sensor>(sensorEntity);

		// Apply the effect only on dynamic elements
		if (bodyInterface.GetMotionType(objectId) != EMotionType::Dynamic)
			break;

		// Apply the effect which corresond to the sensor type
		if (sensor.type == Sensor::Pusher)
			ApplyPusherEffect(deltaTime, sensor, objectId);
		else if (sensor.type == Sensor::Slow)
			ApplySlowEffect(deltaTime, objectId);
		else if (sensor.type == Sensor::Bounce)
			ApplyBounceEffect(deltaTime, sensor, objectId);
	}
}

void SensorSystem::ApplyPusherEffect(const double deltaTime, const Sensor& sensor, const BodyID objectId)
{
	static auto& bodyInterface = JoltSystem::GetBodyInterface();
	static constexpr float SPEED_LIMIT = 400.0f;

	if (bodyInterface.GetLinearVelocity(objectId).Length() < SPEED_LIMIT)
	{
		const Vec3 impulse = sensor.direction * sensor.pushStrength;
		bodyInterface.AddForce(objectId, impulse * WORLD_SIZE_FACTOR);
	}
}

void SensorSystem::ApplyBounceEffect(const double deltaTime, const Sensor& sensor, const BodyID objectId)
{
	static auto& bodyInterface = JoltSystem::GetBodyInterface();

	const Vec3 objectForward = bodyInterface.GetRotation(objectId).RotateAxisZ();

	Vec3 velocity = bodyInterface.GetLinearVelocity(objectId);
	velocity += -objectForward * sensor.pushStrength;

	bodyInterface.SetLinearVelocity(objectId, velocity);
}

void SensorSystem::ApplySlowEffect(const double deltaTime, const BodyID objectId)
{
	static constexpr float HALF_PLAYER_SPEED = 100.0f;
	static auto& bodyInterface = JoltSystem::GetBodyInterface();

	const Vec3 objectVelocity = bodyInterface.GetLinearVelocity(objectId);
	if (objectVelocity.Length() > HALF_PLAYER_SPEED)
	{
		const Vec3 objectForward = bodyInterface.GetRotation(objectId).RotateAxisZ();
		const Vec3 targetVelocity = objectForward * HALF_PLAYER_SPEED;

		static constexpr float SLOW_DOWN_FACTOR = -15.0f;
		const float velocityReducerFactor = 1.0f - exp(SLOW_DOWN_FACTOR * static_cast<float>(deltaTime));
		const Vec3 newVelocity = objectVelocity + (targetVelocity - objectVelocity) * velocityReducerFactor;

		bodyInterface.SetLinearVelocity(objectId, newVelocity);
	}
}
