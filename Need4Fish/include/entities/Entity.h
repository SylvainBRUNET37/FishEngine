#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>

struct Entity
{
	using Index = uint32_t;
	using Generation = uint32_t;

	Index index{};
	Generation generation{}; // Used to reuse index and check if the entity is dead
};

inline bool operator==(const Entity& e1, const Entity& e2)
{
	return e1.index == e2.index &&
		e1.generation == e2.generation;
}

static constexpr Entity INVALID_ENTITY = {.index = UINT32_MAX, .generation = UINT32_MAX};

#endif
