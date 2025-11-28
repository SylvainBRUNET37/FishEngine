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

void ComponentFactory::CreateRigidBody(const nlohmann::json& componentData, EntityManager& entityManager,
                                       const Entity& entity)
{
	const auto& transform = entityManager.Get<Transform>(entity);

	const string type = componentData["type"];

	if (type == "meshShape" || type == "boxShape")
	{
		const auto& meshInstance = entityManager.Get<MeshInstance>(entity);
		const auto& mesh = Locator::Get<ResourceManager>().GetMesh(meshInstance.meshIndex);

	if (type == "boxShape")
		entityManager.AddComponent<RigidBody>(entity,
			ShapeFactory::CreateCube(transform, mesh));
		else if (type == "meshShape")
			entityManager.AddComponent<RigidBody>(entity,
				ShapeFactory::CreateMeshShape(transform, mesh));

	}
	else if (type == "currentSensor")
		entityManager.AddComponent<RigidBody>(entity,
			SensorFactory::CreateCubeCurrentSensor(transform));
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
