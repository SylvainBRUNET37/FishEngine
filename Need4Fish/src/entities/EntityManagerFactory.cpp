#include "pch.h"
#include "entities/EntityManagerFactory.h"

#include "components/Components.h"
#include "resources/ComponentLoader.h"

std::unique_ptr<EntityManager> EntityManagerFactory::Create(const SceneResource& sceneResource)
{
	auto entityManager = std::make_unique<EntityManager>();

	// Create an entity for every node
	const size_t nbEntities = sceneResource.nodes.size();
	std::vector<Entity> entities;
	entities.reserve(nbEntities);
	for (size_t i = 0; i < nbEntities; ++i)
	{
		entities.push_back(entityManager->CreateEntity());
	}

	// Add componentPools and build hierarchy
	size_t nodeIndex = 0;
	const Entity rootEntity = entities[0];
	for (; nodeIndex < sceneResource.nodes.size(); ++nodeIndex)
	{
		const auto& node = sceneResource.nodes[nodeIndex];
		const auto entity = entities[nodeIndex];

		entityManager->AddComponent<Transform>(entity, node.transform);
		entityManager->AddComponent<Name>(entity, node.name);

		if (node.meshIndex != UINT32_MAX)
		{
			// Create a distortion mesh if the node is a sensor (not the best way of linking, but it works)
			if (node.name.find("Sensor") != std::string::npos)
				entityManager->AddComponent<DistortionMeshInstance>(entity, DistortionMeshInstance{ .meshIndex = node.meshIndex });
			else
				entityManager->AddComponent<MeshInstance>(entity, MeshInstance{ .meshIndex = node.meshIndex });
		}

		// Set parent entity to node parent or root entity if he is orphan
		const Entity parentEntity = node.parentIndex == UINT32_MAX ? rootEntity : entities[node.parentIndex];
		entityManager->AddComponent<Hierarchy>(entity, Hierarchy{ parentEntity, {} });

		// Add the children to the parent
		auto& parentHierarchy = entityManager->Get<Hierarchy>(parentEntity);
		parentHierarchy.children.push_back(entity);

		if (not node.componentsDatas.empty())
			ComponentLoader::LoadComponent(node.componentsDatas, *entityManager, entity);
	}

	for (size_t pointLightIndex = 0; pointLightIndex < sceneResource.pointLights.size(); ++pointLightIndex)
	{
		const auto& light = sceneResource.pointLights[pointLightIndex];
		const auto entity = entities[light.nodeId];

		entityManager->AddComponent<PointLight>(entity, light);
	}

	return std::move(entityManager);
}
