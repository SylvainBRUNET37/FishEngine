#ifndef ENTITY_MANAGER_FACTORY_H
#define ENTITY_MANAGER_FACTORY_H

#include "entities/EntityManager.h"
#include "resources/SceneResource.h"

class EntityManagerFactory
{
public:
	[[nodiscard]] static EntityManager Create()
	{
		return EntityManager{};
	}

	[[nodiscard]] static EntityManager Create22222(const SceneResource& sceneResource);

	// TODO: delete
	[[nodiscard]] static EntityManager Create(const SceneResource& sceneResource);
};

#endif
