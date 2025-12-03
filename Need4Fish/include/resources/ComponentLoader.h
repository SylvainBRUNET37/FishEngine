#ifndef COMPONENT_LOADER_H
#define COMPONENT_LOADER_H

#include <string>
#include <string_view>
#include <array>

#include "components/Components.h"
#include "components/ComponentFactory.h"
#include "json_fwd.hpp"

struct Entity;
class EntityManager;

class ComponentLoader
{
public:
	static void LoadComponent(const std::string& componentData, EntityManager& entityManager, const Entity& entity);

private:
	using ComponentFactoryMethod = void(*)(const nlohmann::json& componentData,
	                                       EntityManager& entityManager,
	                                       const Entity& entity);

	using ComponentFactoryMethods =
	std::array<std::pair<std::string_view, ComponentFactoryMethod>, std::tuple_size_v<Components>>;

	static constexpr ComponentFactoryMethods componentFactoryMethods =
	{
		{
			{
				"rigidBody",
				[](const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity)
				{
					ComponentFactory::CreateRigidBody(componentData, entityManager, entity);
				}
			},
			{
				"eatable",
				[](const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity)
				{
					ComponentFactory::CreateEatable(componentData, entityManager, entity);
				}
			},
			{
				"controllable",
				[](const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity)
				{
					ComponentFactory::CreateControllable(componentData, entityManager, entity);
				}
			},
			{
				"powerSource",
				[](const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity)
				{
					ComponentFactory::CreatePowerSource(componentData, entityManager, entity);
				}
			},
			{
				"sensor",
				[](const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity)
				{
					ComponentFactory::CreateSensor(componentData, entityManager, entity);
				}
			},
		}
	};
};

#endif
