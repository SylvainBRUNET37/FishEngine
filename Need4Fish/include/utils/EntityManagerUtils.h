#ifndef ENTITY_MANAGER_UTILS_H
#define ENTITY_MANAGER_UTILS_H

#include <optional>

#include "entities/Entity.h"

class EntityManager;

namespace EntityManagerUtils
{
    [[nodiscard]] std::optional<Entity> GetEntityFromBody(EntityManager& entityManager, const JPH::BodyID& bodyId);
}

#endif