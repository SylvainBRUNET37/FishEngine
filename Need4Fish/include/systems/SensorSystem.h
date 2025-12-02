#ifndef SENSOR_SYSTEM_H
#define SENSOR_SYSTEM_H

#include "System.h"

class SensorSystem : public System
{
public:
	// The sensor should be the first of the pair
	static void AddContact(std::pair<JPH::BodyID, JPH::BodyID> contact);
	static void RemoveContact(const std::pair<JPH::BodyID, JPH::BodyID>& contact);

private:
	static constexpr float WORLD_SIZE_FACTOR = 30000000;

	// The sensor if the first of the pair
	inline static std::vector<std::pair<JPH::BodyID, JPH::BodyID>> activeContacts;

	void Update(double deltaTime, EntityManager& entityManager) override;

	static void ApplyPusherEffect(double deltaTime, const Sensor& sensor, JPH::BodyID objectId);
	static void ApplySeaweedEffect(double deltaTime, const Sensor& sensor, JPH::BodyID objectId);
};

#endif

