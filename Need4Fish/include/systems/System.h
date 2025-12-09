#ifndef SYSTEM_H
#define SYSTEM_H

#include "entities/EntityManager.h"

// Contains the logic that operate on different component
struct System
{
	System() = default;
	virtual ~System() = default;

	System(const System&) = default;
	System(System&&) = default;
	System& operator=(const System&) = default;
	System& operator=(System&&) = default;

	virtual void Update(double deltaTime, EntityManager& entityManager) = 0;
	virtual void Reset() {}
};

#endif
