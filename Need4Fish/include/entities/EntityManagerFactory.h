#ifndef ENTITY_MANAGER_FACTORY_H
#define ENTITY_MANAGER_FACTORY_H

#include "entities/EntityManager.h"
#include "resources/SceneResource.h"

class EntityManagerFactory
{
public:
	[[nodiscard]] static std::unique_ptr<EntityManager> Create(const SceneResource& sceneResource);
};

#endif
