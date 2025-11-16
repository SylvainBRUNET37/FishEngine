#ifndef RIGID_BODY_FACTORY_H
#define RIGID_BODY_FACTORY_H

#include "json_fwd.hpp"

class EntityManager;
struct Entity;

class RigidBodyFactory
{
public:
	static void CreateRigidBody(const nlohmann::json& componentData, EntityManager& entityManager, const Entity& entity);
};

#endif