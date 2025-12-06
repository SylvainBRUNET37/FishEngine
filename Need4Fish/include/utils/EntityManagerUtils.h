#ifndef ENTITY_MANAGER_UTILS_H
#define ENTITY_MANAGER_UTILS_H

#include "entities/EntityManagerFactory.h"

namespace EntityManagerUtils
{
    [[nodiscard]] std::optional<Entity> GetEntityFromBody(EntityManager& entityManager, const JPH::BodyID& bodyId);
}

#endif