#ifndef POWER_SOURCE_H
#define POWER_SOURCE_H

#include <cstdint>

// You can add values but do not change the associated number
enum class Power : std::uint8_t
{
	Drug = 0
};

struct PowerSource
{
	Power power;
	double effectDuration; // Total duration of the effect
	double timeWithEffect; // Time spent with the effect
};

#endif
