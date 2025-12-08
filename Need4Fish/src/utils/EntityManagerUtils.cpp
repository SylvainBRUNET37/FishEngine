#include "pch.h"
#include "utils/EntityManagerUtils.h"
#include <optional>
#include <ranges>

using namespace std;
using namespace JPH;

std::optional<Entity> EntityManagerUtils::GetEntityFromBody(EntityManager& entityManager, const BodyID& bodyId)
{
    const auto eatables = entityManager.View<RigidBody>();

	const auto it = ranges::find_if
	(
		eatables,
		[&](const auto& tup)
		{
			const auto& [entity, rigidBody] = tup;
			return rigidBody.body->GetID() == bodyId;
		}
	);


	if (it != eatables.end())
	{
		const auto& [entity, _] = *it;
		return entity;
	}
	return std::nullopt;
}
