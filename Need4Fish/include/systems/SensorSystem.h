#ifndef SENSOR_SYSTEM_H
#define SENSOR_SYSTEM_H

#include "System.h"

class SensorSystem : public System
{
public:
	// The sensor should be the first of the pair
	static void AddContact(std::pair<JPH::BodyID, JPH::BodyID> contact) { activeContacts.emplace_back(contact); }
	static void RemoveContact(const std::pair<JPH::BodyID, JPH::BodyID>& contact);

private:
	// The sensor if the first of the pair
	inline static std::vector<std::pair<JPH::BodyID, JPH::BodyID>> activeContacts;

	void Update(double deltaTime, EntityManager& entityManager) override;
};

#endif

