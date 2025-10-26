#ifndef ENTITY_MANAGER_FACTORY_H
#define ENTITY_MANAGER_FACTORY_H

#include "ResourceManager.h"
#include "rendering/core/Transform.h"
#include "rendering/ecs/EntityManager.h"

template <typename... Components>
class EntityManagerFactory
{
public:
	[[nodiscard]] static EntityManager<Components...> Create()
	{
		return EntityManager<Components...>{};
	}

	[[nodiscard]] static EntityManager<Components...> Create(const SceneResource& sceneResource)
	{
		EntityManager<Components...> entityManager{};

		// Create the root entity
		const auto rootEntity = entityManager.CreateEntity();
		entityManager.template AddComponent<Hierarchy>(rootEntity, Hierarchy{INVALID_ENTITY, {}});

		// Create an entity for every node
		std::vector<Entity> entities;
		entities.reserve(sceneResource.nodes.size());
		for (size_t i = 0; i < sceneResource.nodes.size(); ++i)
		{
			entities.push_back(entityManager.CreateEntity());
		}

		// Add components and build hierarchy
		for (size_t i = 0; i < sceneResource.nodes.size(); ++i)
		{
			const auto& node = sceneResource.nodes[i];
			const auto entity = entities[i];

			entityManager.template AddComponent<Transform>(entity, node.transform);

			if (node.meshIndex != UINT32_MAX)
				entityManager.template AddComponent<Mesh>(entity, sceneResource.meshes[node.meshIndex]);

			// Set parent entity to node parent or root entity if he is orphan
			const Entity parentEntity = node.parentIndex == UINT32_MAX ? rootEntity : entities[node.parentIndex];
			entityManager.template AddComponent<Hierarchy>(entity, Hierarchy{parentEntity, {}});

			// Add the children to the parent
			auto& parentHierarchy = entityManager.template Get<Hierarchy>(parentEntity);
			parentHierarchy.children.push_back(entity);
		}

		return entityManager;
	}
};

#endif
