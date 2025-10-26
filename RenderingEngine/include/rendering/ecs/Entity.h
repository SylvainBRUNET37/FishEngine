#ifndef ENTITY_H
#define ENTITY_H

struct Entity
{
	using Index = uint32_t;
	using Generation = uint32_t;

	Index index{};
	Generation generation{};
};

static constexpr Entity INVALID_ENTITY = {.index = UINT32_MAX, .generation = UINT32_MAX};

#endif
