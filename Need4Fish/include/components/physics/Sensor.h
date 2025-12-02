#ifndef SENSOR_H
#define SENSOR_H

#include <cstdint>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

// Sensor which affect the position of the user
struct Sensor
{
	enum Type : std::uint8_t
	{
		Pusher = 0, // push (geyser, current)
		Seaweed = 1 // slow
	};

	JPH::Vec3 direction{}; // the push direction
	JPH::Body* body;
	float pushStrength{};
	Type type;
};

#endif