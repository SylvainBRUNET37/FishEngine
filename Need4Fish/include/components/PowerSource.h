#ifndef POWER_SOURCE_H
#define POWER_SOURCE_H

#include <cstdint>

#include "entities/Entity.h"

// You can add values but do not change the associated number
enum class Power : std::uint8_t
{
	Drug = 0,
	Floats = 1,
	Sink = 2
};

struct PowerSource
{
	Power power;
	double effectDuration; // Total duration of the effect
	double timeWithEffect; // Time spent with the effect
	Entity target = INVALID_ENTITY;
};

#endif
