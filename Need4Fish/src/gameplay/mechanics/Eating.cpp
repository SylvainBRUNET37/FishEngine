#include "pch.h"
#include "gameplay/mechanics/Eating.h"
#include <Jolt/Physics/Body/Body.h>
#include "GameEngine.h"
#include "DirectXMath.h"
#include "components/PowerSource.h"
#include "systems/PowerSystem.h"
#include <cmath>
#include "entities/EntityManager.h"

using namespace DirectX;

static void KillRecursively(EntityManager& entityManager, const Entity preyEntity)
{
	if (entityManager.HasComponent<Hierarchy>(preyEntity))
	{
		const auto& hierarchy = entityManager.Get<Hierarchy>(preyEntity);

		for (const Entity child : hierarchy.children)
			KillRecursively(entityManager, child);
	}

	entityManager.Kill(preyEntity);
}

[[nodiscard]] static float CalculateGrowthFactor(const float predatorMass, const float preyMass)
{
	// Linear proportionnal growth
	auto MAX = 1400.0f; // when this is reached, mass continues to grow but not the mesh / hitbox

	if (predatorMass > MAX)
		return 0.0f;

	auto newMass = predatorMass + preyMass;
	return (newMass <= MAX ? newMass : MAX) / predatorMass;
}


[[nodiscard]] static std::optional<std::tuple<const Entity, Eatable&, RigidBody&>> GetEatableAndRigidBodyFromBody(
	EntityManager& entityManager,
	JPH::BodyID bodyId
)
{
	auto eatables = entityManager.View<Eatable, RigidBody>();
	auto it = std::find_if(
		eatables.begin(),
		eatables.end(),
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

[[nodiscard]] static bool IsEntityAPlayer(EntityManager& entityManager, Entity SearchedEntity)
{
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

// TODO: utiliser des getters dans l'ECS plut�t qu'avoir 280000 arguments
static void AcuallyEat(
	EntityManager& entityManager,
	const Entity& predatorEntity,
	Eatable& predatorEatable,
	RigidBody& predatorBody,
	const Entity& preyEntity,
	Eatable& preyEatable
)
{
	const float GROWTH_RATE = 0.6;
	auto weightedMass = preyEatable.mass * GROWTH_RATE;

	float scaleFactor = CalculateGrowthFactor(predatorEatable.mass, weightedMass);

	// Apply power effect of the killed entity
	if (entityManager.HasComponent<PowerSource>(preyEntity))
		PowerSystem::AddEffect(entityManager.Get<PowerSource>(preyEntity));
	
	// Eat and kill
	if (preyEatable.isApex) GameState::currentState = GameState::WON;
	predatorEatable.mass += weightedMass;

	KillRecursively(entityManager, preyEntity);

	if (scaleFactor < 0.01)
		return;

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

	CameraSystem::ScaleCamera(scaleFactor);

	// Scale mesh
	auto& trans = entityManager.Get<Transform>(predatorEntity);
	// We should be able to assume scaleFactor - scale > 0, but better safe than sorry
	auto a = scaleFactor * trans.scale.x - trans.scale.x - trans.deltaScale.x;
	trans.deltaScale.x += std::abs(scaleFactor * (trans.scale.x + trans.deltaScale.x) - trans.scale.x - trans.deltaScale.x);
	trans.deltaScale.y += std::abs(scaleFactor * (trans.scale.y + trans.deltaScale.y) - trans.scale.y - trans.deltaScale.y);
	trans.deltaScale.z += std::abs(scaleFactor * (trans.scale.z + trans.deltaScale.z) - trans.scale.z - trans.deltaScale.z);

	const float F_GROWTH_STEPS = 60.0f;

	trans.scaleStep = trans.deltaScale.x / F_GROWTH_STEPS;
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
	else if (IsEntityAPlayer(entityManager, predatorEntity))
	{
		GameState::isGrowing = true;
		AcuallyEat(entityManager, predatorEntity, predatorEatable, predatorBody, preyEntity, preyEatable);
	}
	// Else : nothing (fishes do not eat each other)
}


void Eating::Eat(EntityManager& entityManager, JPH::BodyID bodyId1, JPH::BodyID bodyId2)
{
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

void Eating::UpdatePlayerScale(EntityManager& entityManager)
{

	for (const auto& [entity, _, transform] : entityManager.View<Controllable, Transform>())
	{
		if (XMVector3NotEqual(
				XMLoadFloat3(&transform.deltaScale),
				XMVectorZero()
			)
		)
		{
			float* delta = &transform.deltaScale.x;
			float* scale = &transform.scale.x;
			for (int i = 0; i < 3; ++i) {
				if (delta[i] < transform.scaleStep) {
					scale[i] += delta[i];
					delta[i] = 0.0f;
				}
				else {
					delta[i] -= transform.scaleStep;
					scale[i] += transform.scaleStep;
				}
			}

			// Mise à jour progressive de la caméra
			auto applyDelta = [](float& target, float& delta, float step) {
				if (std::abs(delta) < std::abs(step)) {
					target += delta;
					delta = 0.0f;
				}
				else {
					const float actualStep = std::copysign(step, delta);
					delta -= actualStep;
					target += actualStep;
				}
				};

			applyDelta(Camera::distance, Camera::deltaDistance, Camera::cameraScaleStep);
			applyDelta(Camera::heightOffset, Camera::deltaHeightOffset, Camera::cameraScaleStep);
			applyDelta(Camera::minDistance, Camera::deltaMinDistance, Camera::cameraScaleStep);
			applyDelta(Camera::maxDistance, Camera::deltaMaxDistance, Camera::cameraScaleStep);
			applyDelta(Camera::zoomSpeed, Camera::deltaZoomSpeed, Camera::cameraScaleStep);
		}
		else
		{
			GameState::isGrowing = false;
		}
	}
}
