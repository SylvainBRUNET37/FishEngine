#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <tuple>
#include <type_traits>
#include <concepts>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "ComponentPool.h"
#include "ecs/Entity.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"

struct Hierarchy
{
	Entity parent = INVALID_ENTITY;
	std::vector<Entity> children{};
};

struct RigidBody
{
	JPH::Body* body;
};

using Components = std::tuple
<
	Mesh,
	RigidBody,
	Transform,
	Hierarchy
>;

template<typename Tuple> struct ComponentPoolsFromTuple;
template<typename... Ts>
struct ComponentPoolsFromTuple<std::tuple<Ts...>>
{
	using type = std::tuple<ComponentPool<Ts>...>;
};

using ComponentPools = ComponentPoolsFromTuple<Components>::type;

template <typename T, typename Tuple>
struct IsOneOf;

template <typename T, typename... Ts>
struct IsOneOf<T, std::tuple<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};

template <typename T>
concept OneOf = IsOneOf<T, Components>::value;

#endif
