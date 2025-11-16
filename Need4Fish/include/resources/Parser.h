#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <string_view>
#include <array>

#include "components/Components.h"
#include "components/factories/RigidBodyFactory.h"

#include "json_fwd.hpp"

struct Entity;
class EntityManager;

class Parser
{
public:
	static void Parse(const std::string& componentData, EntityManager& entityManager, const Entity& entity);

private:
	using ComponentFactoryMethod = void(*)(const nlohmann::json& componentData, EntityManager& entityManager,
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
					RigidBodyFactory::CreateRigidBody(componentData, entityManager, entity);
				}
			},
		}
	};
};

#endif
