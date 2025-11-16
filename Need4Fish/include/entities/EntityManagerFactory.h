#ifndef ENTITY_MANAGER_FACTORY_H
#define ENTITY_MANAGER_FACTORY_H

#include "components/Components.h"
#include "rendering/core/Transform.h"
#include "entities/EntityManager.h"

class EntityManagerFactory
{
public:
	[[nodiscard]] static EntityManager Create()
	{
		return EntityManager{};
	}

	[[nodiscard]] static EntityManager Create(const SceneResource& sceneResource)
	{
		EntityManager entityManager{};

		// Create the root entity
		const auto rootEntity = entityManager.CreateEntity();
		entityManager.AddComponent<Hierarchy>(rootEntity, Hierarchy{INVALID_ENTITY, {}});

		// Create an entity for every node
		const size_t nbEntities = sceneResource.nodes.size() + sceneResource.pointLights.size();
		std::vector<Entity> entities;
		entities.reserve(nbEntities);
		for (size_t i = 0; i < nbEntities; ++i)
		{
			entities.push_back(entityManager.CreateEntity());
		}

		// Add componentPools and build hierarchy
		size_t nodeIndex = 0;
		for (; nodeIndex < sceneResource.nodes.size(); ++nodeIndex)
		{
			const auto& node = sceneResource.nodes[nodeIndex];
			const auto entity = entities[nodeIndex];

			entityManager.AddComponent<Transform>(entity, node.transform);
			entityManager.AddComponent<Name>(entity, node.name);

			if (node.meshIndex != UINT32_MAX)
				entityManager.AddComponent<Mesh>(entity, sceneResource.meshes[node.meshIndex]);

			// Set parent entity to node parent or root entity if he is orphan
			const Entity parentEntity = node.parentIndex == UINT32_MAX ? rootEntity : entities[node.parentIndex];
			entityManager.AddComponent<Hierarchy>(entity, Hierarchy{parentEntity, {}});

			// Add the children to the parent
			auto& parentHierarchy = entityManager.Get<Hierarchy>(parentEntity);
			parentHierarchy.children.push_back(entity);
		}

		for (size_t pointLightIndex = 0; pointLightIndex < sceneResource.pointLights.size(); ++pointLightIndex)
		{
			const auto& light = sceneResource.pointLights[pointLightIndex];
			const auto entity = entities[nodeIndex + pointLightIndex];

			entityManager.AddComponent<PointLight>(entity, light);
		}

		return entityManager;
	}
};

#endif
