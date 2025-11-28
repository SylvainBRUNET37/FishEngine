#ifndef SENSOR_H
#define SENSOR_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

// Sensor which affect the position of the user
struct Sensor
{
	JPH::Vec3 direction{};
	JPH::Body* body;
	float pushStrength{};
};

#endif