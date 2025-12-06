#ifndef EATING_H
#define EATING_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

class EntityManager;

namespace Eating
{
	void Eat(EntityManager& entityManager, JPH::BodyID bodyId1, JPH::BodyID bodyId2);

	void UpdatePlayerScale(EntityManager& entityManager);
}

#endif