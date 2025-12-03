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
		Pusher = 0, // geyser, current
		Slow = 1,
		Bounce = 2,
	};

	JPH::Vec3 direction{}; // the push direction
	JPH::Body* body;
	float pushStrength{};
	Type type;
};

#endif