#include "pch.h"
#include "components/ComponentFactory.h"

#include "components/physics/RigidBody.h"
#include "PhysicsEngine/ShapeFactory.h"
#include "entities/EntityManager.h"

#include "json.hpp"
#include "Locator.h"
#include "physicsEngine/SensorFactory.h"
#include "resources/ResourceManager.h"

using namespace std;
using namespace JPH;

void ComponentFactory::CreateRigidBody(const nlohmann::json& componentData, EntityManager& entityManager,
                                       const Entity& entity)
{
	const auto& transform = entityManager.Get<Transform>(entity);
	const auto& meshInstance = entityManager.Get<MeshInstance>(entity);
	const auto& name = entityManager.Get<Name>(entity);
	const auto& mesh = Locator::Get<ResourceManager>().GetMesh(meshInstance.meshIndex);

	if (componentData["type"] == "boxShape")
		entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateCube(transform, mesh));
	else if (componentData["type"] == "meshShape")
		entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateMeshShape(transform, mesh));
	else if (componentData["type"] == "hullShape")
		entityManager.AddComponent<RigidBody>(entity, ShapeFactory::CreateConvexHullShape(transform, mesh, !componentData.contains("isDecor")));
}

void ComponentFactory::CreateSensor(const nlohmann::json& componentData, EntityManager& entityManager,
                                    const Entity& entity)
{
	const auto& transform = entityManager.Get<Transform>(entity);

	const auto type = static_cast<Sensor::Type>(componentData["type"].get<int>());
	const auto pushStrength = componentData["pushStrength"].get<float>();
	const Vec3 direction =
	{
			componentData["direction"]["x"].get<float>(),
			componentData["direction"]["y"].get<float>(),
			componentData["direction"]["z"].get<float>()
	};

	Body* body = nullptr;
	if (componentData["bodyType"] == "boxSensor")
	{
		if (entityManager.HasComponent<MeshInstance>(entity))
		{
			const auto& meshInstance = entityManager.Get<MeshInstance>(entity);
			const auto& mesh = Locator::Get<ResourceManager>().GetMesh(meshInstance.meshIndex);
			body = SensorFactory::CreateCubeCurrentSensor(transform, mesh, entity);
		}
		else
			body = SensorFactory::CreateCubeCurrentSensor(transform, entity);
	}
	else if (componentData["bodyType"] == "cylinderSensor")
		body = SensorFactory::CreateCylinderSensor(transform, entity);
	
	vassert(body, "Sensor must have a body type");

	entityManager.AddComponent<Sensor>(entity, 
		Sensor{
		direction,
		body,
		pushStrength,
		type
	});
}

void ComponentFactory::CreateEatable(const nlohmann::json& componentData, EntityManager& entityManager,
                                     const Entity& entity)
{
	auto& eatableComponent = entityManager.AddComponent<Eatable>(entity, componentData["mass"].get<float>());
	eatableComponent.isApex = componentData["isApex"].get<bool>();
}

void ComponentFactory::CreateControllable(const nlohmann::json& componentData, EntityManager& entityManager,
                                          const Entity& entity)
{
	entityManager.AddComponent<Controllable>(entity, componentData["maxSpeed"].get<float>());
}

void ComponentFactory::CreatePowerSource(const nlohmann::json& componentData, EntityManager& entityManager,
                                         const Entity& entity)
{
	auto& powerSource = entityManager.AddComponent<PowerSource>(entity, PowerSource{});
	powerSource.power = static_cast<Power>(componentData["power"].get<int>());
	powerSource.effectDuration = componentData["effectDuration"].get<double>();
}

void ComponentFactory::CreateAIController(const nlohmann::json& componentData, EntityManager& entityManager,
	const Entity& entity)
{
	auto& aiController = entityManager.AddComponent<AIController>(entity, AIController{});

	aiController.maxSpeed = componentData["maxSpeed"].get<double>();
	aiController.acceleration = componentData["acceleration"].get<double>();
	aiController.safeDistance = componentData["safeDistance"].get<double>();
}
