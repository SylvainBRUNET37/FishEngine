#include "pch.h"
#include "gameplay/mechanics/Eating.h"
#include <Jolt/Physics/Body/Body.h>
#include "GameEngine.h"

static std::optional<std::tuple<const Entity, Eatable&, RigidBody&>> GetEatableAndRigidBodyFromBody(
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

static bool IsEntityAPlayer(EntityManager& entityManager, Entity SearchedEntity) {
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
				if (IsEntityAPlayer(entityManager, firstEntity))
				{
					//InitGame();
					GameState::currentState = GameState::DIED;
				}
				else
				{
					if (firstEatable.isApex) GameState::currentState = GameState::WON;
					entityManager.Kill(firstEntity);
					secondEatable.mass += firstEatable.mass;

					// Shape scaling test
					auto currentShape = secondBody.body->GetShape();
					auto& bodyInterface = JoltSystem::GetPhysicSystem().GetBodyInterface();
					JPH::Vec3 scale(2.0f, 2.0f, 2.0f);   // test scale // TODO changer ça + fonction
					if (currentShape->GetSubType() == JPH::EShapeSubType::Box)
					{
						const JPH::BoxShape* box = static_cast<const JPH::BoxShape*>(currentShape);

						JPH::Vec3 newHalfExtents = box->GetHalfExtent() * scale;

						JPH::RefConst<JPH::Shape> newShape = new JPH::BoxShape(newHalfExtents);

						bodyInterface.SetShape(bodyId2, newShape, true, JPH::EActivation::Activate);
					}
					// Scale mesh
					auto& trans = entityManager.Get<Transform>(secondEntity);
					trans.scale.x *= 2;
					trans.scale.y *= 2;
					trans.scale.z *= 2;
				}
			}
			else if (secondEatable.CanBeEatenBy(firstEatable)) {
				if (IsEntityAPlayer(entityManager, secondEntity))
				{
					//InitGame();
					GameState::currentState = GameState::DIED;
				}
				else
				{
					if (secondEatable.isApex) GameState::currentState = GameState::WON;
					entityManager.Kill(secondEntity);
					firstEatable.mass += secondEatable.mass;

					// Shape scaling test
					auto currentShape = firstBody.body->GetShape();
					auto& bodyInterface = JoltSystem::GetPhysicSystem().GetBodyInterface();
					JPH::Vec3 scale(2.0f, 2.0f, 2.0f);   // test scale // TODO: changer ça + fonction
					if (currentShape->GetSubType() == JPH::EShapeSubType::Box)
					{
						const JPH::BoxShape* box = static_cast<const JPH::BoxShape*>(currentShape);

						JPH::Vec3 newHalfExtents = box->GetHalfExtent() * scale;

						JPH::RefConst<JPH::Shape> newShape = new JPH::BoxShape(newHalfExtents);

						bodyInterface.SetShape(bodyId1, newShape, true, JPH::EActivation::Activate);
					}
					// Scale mesh
					auto& mesh = entityManager.Get<Mesh>(firstEntity);
					auto& trans = entityManager.Get<Transform>(firstEntity);
					trans.scale.x *= 2;
					trans.scale.y *= 2;
					trans.scale.z *= 2;
				}
			}
		}
}
