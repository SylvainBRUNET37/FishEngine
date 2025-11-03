#ifndef SYSTEM_H
#define SYSTEM_H

#include "entities/EntityManager.h"

// Contains the logic that operate on different component
struct System
{
	System() = default;
	virtual ~System() = default;

	System(const System&) = delete;
	System(System&&) = delete;
	System& operator=(const System&) = delete;
	System& operator=(System&&) = delete;

	virtual void Update(double deltaTime, EntityManager& entityManager) = 0;
};

#endif
