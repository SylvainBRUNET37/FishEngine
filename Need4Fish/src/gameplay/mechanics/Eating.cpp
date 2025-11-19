#include "pch.h"
#include "gameplay/mechanics/Eating.h"
#include <Jolt/Physics/Body/Body.h>
#include "GameEngine.h"

[[nodiscard]] static float CalculateGrowthFactor(const float predatorMass, const float preyMass) {
	// Linear proportionnal growth
	return (predatorMass + preyMass) / predatorMass;
}


[[nodiscard]] static std::optional<std::tuple<const Entity, Eatable&, RigidBody&>> GetEatableAndRigidBodyFromBody(
	EntityManager& entityManager,
	JPH::BodyID bodyId
) {
	auto eatables = entityManager.View<Eatable, RigidBody>();
	auto it = std::find_if(eatables.begin(), eatables.end(),
		[&](auto&& tuple)
		{
			auto& [entity, eatable, rigidBody] = tuple;
			return rigidBody.body->GetID() == bodyId;
		});
	if (it != eatables.end())
	{
		return std::tuple<Entity, Eatable&, RigidBody&>(*it);
	}
	return std::nullopt;
}


[[nodiscard]] static bool IsEntityAPlayer(EntityManager& entityManager, Entity SearchedEntity) {
	auto watchables = entityManager.View<Controllable>();
	auto it = std::find_if(
		watchables.begin(),
		watchables.end(),
		[&](auto&& tuple)
		{
			auto& [entity, _] = tuple;
			return entity.index == SearchedEntity.index;
		});
	return it != watchables.end();
}

// TODO: utiliser des getters dans l'ECS plutôt qu'avoir 280000 arguments
static void AcuallyEat(
	EntityManager& entityManager,
	const Entity& predatorEntity,
	Eatable& predatorEatable,
	RigidBody& predatorBody,
	const Entity& preyEntity,
	Eatable& preyEatable
)
{
	float scaleFactor = CalculateGrowthFactor(predatorEatable.mass, preyEatable.mass);
	
	// Eat and kill
	if (preyEatable.isApex) GameState::currentState = GameState::WON;
	predatorEatable.mass += preyEatable.mass;
	entityManager.Kill(preyEntity);

	// Grow the hitbox
	auto currentShape = predatorBody.body->GetShape();
	auto& bodyInterface = JoltSystem::GetPhysicSystem().GetBodyInterface();
	JPH::Vec3 scale(scaleFactor, scaleFactor, scaleFactor);
	if (currentShape->GetSubType() == JPH::EShapeSubType::Box)
	{
		const JPH::BoxShape* box = static_cast<const JPH::BoxShape*>(currentShape);
		JPH::Vec3 newHalfExtents = box->GetHalfExtent() * scale;
		JPH::RefConst<JPH::Shape> newShape = new JPH::BoxShape(newHalfExtents);
		bodyInterface.SetShape(predatorBody.body->GetID(), newShape, true, JPH::EActivation::Activate);
	}

	// Scale mesh
	auto& trans = entityManager.Get<Transform>(predatorEntity);
	trans.scale.x *= scaleFactor;
	trans.scale.y *= scaleFactor;
	trans.scale.z *= scaleFactor;
}

static void LoseOrEat(
	EntityManager& entityManager,
	const Entity& predatorEntity,
	Eatable& predatorEatable,
	RigidBody& predatorBody,
	const Entity& preyEntity,
	Eatable& preyEatable
)
{
	if (IsEntityAPlayer(entityManager, preyEntity))
	{
		GameState::currentState = GameState::DIED;
	}
	else
	{
		AcuallyEat(entityManager, predatorEntity, predatorEatable, predatorBody, preyEntity, preyEatable);
	}
}


void Eating::Eat(EntityManager& entityManager, JPH::BodyID bodyId1, JPH::BodyID bodyId2) {
		// Check if the both objects is eatable
		auto firstObject = GetEatableAndRigidBodyFromBody(entityManager, bodyId1);
		auto secondObject = GetEatableAndRigidBodyFromBody(entityManager, bodyId2);

		if (firstObject.has_value() && secondObject.has_value())
		{
			auto& [firstEntity, firstEatable, firstBody] = firstObject.value();
			auto& [secondEntity, secondEatable, secondBody] = secondObject.value();

			// Kill things if necessary
			if (firstEatable.CanBeEatenBy(secondEatable)) {
				LoseOrEat(entityManager, secondEntity, secondEatable, secondBody,  firstEntity, firstEatable);
			}
			else if (secondEatable.CanBeEatenBy(firstEatable)) {
				LoseOrEat(entityManager, firstEntity, firstEatable, firstBody, secondEntity, secondEatable);
			}
		}
}
