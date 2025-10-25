#ifndef ENTITY_H
#define ENTITY_H

struct Entity
{
	using Index = uint32_t;
	using Generation = uint32_t;

	uint32_t index;
	uint32_t generation;
};

#endif
