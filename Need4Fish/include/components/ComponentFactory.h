#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include "json_fwd.hpp"

class EntityManager;
struct Entity;

class ComponentFactory
{
public:
	static void CreateRigidBody(const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity);
	static void CreateEatable(const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity);
	static void CreateControllable(const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity);
	static void CreatePowerSource(const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity);
};

#endif