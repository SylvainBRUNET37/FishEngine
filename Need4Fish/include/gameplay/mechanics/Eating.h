#pragma once
#include "pch.h"
#include "entities/EntityManagerFactory.h"

namespace Eating
{
	void Eat(EntityManager& entityManager, JPH::BodyID bodyId1, JPH::BodyID bodyId2);
	std::optional<Entity> GetEntityFromBody(EntityManager& entityManager, const JPH::BodyID& bodyId);

	void UpdatePlayerScale(EntityManager& entityManager);
}