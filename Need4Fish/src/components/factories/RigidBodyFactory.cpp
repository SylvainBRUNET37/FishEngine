#include "pch.h"
#include "components/factories/RigidBodyFactory.h"

#include "components/physics/RigidBody.h"
#include "PhysicsEngine/ShapeFactory.h"
#include "entities/EntityManager.h"

#include "json.hpp"

void RigidBodyFactory::CreateRigidBody(const nlohmann::json& componentData, EntityManager& entityManager,
                                       const Entity& entity)
{
	const auto& transform = entityManager.Get<Transform>(entity);
	const auto& mesh = entityManager.Get<Mesh>(entity);

	if (componentData["type"] == "meshShape")
		entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateMeshShape(transform, mesh));
}
