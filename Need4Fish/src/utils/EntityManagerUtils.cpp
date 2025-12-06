#include "pch.h"
#include "utils/EntityManagerUtils.h"
#include <optional>

std::optional<Entity> EntityManagerUtils::GetEntityFromBody(EntityManager& entityManager, const JPH::BodyID& bodyId)
{
	auto eatables = entityManager.View<RigidBody>();
	auto it = std::find_if(
		eatables.begin(),
		eatables.end(),
		[&](auto&& tuple)
		{
			auto& [entity, rigidBody] = tuple;
			return rigidBody.body->GetID() == bodyId;
		});
	if (it != eatables.end())
	{
		const auto& [entity, _] = *it;
		return entity;
	}
	return std::nullopt;
}
