#ifndef POWER_SOURCE_H
#define POWER_SOURCE_H

#include <cstdint>

enum class Power : uint8_t
{
	Drug
};

struct PowerSource
{
	Power power;
};

#endif
