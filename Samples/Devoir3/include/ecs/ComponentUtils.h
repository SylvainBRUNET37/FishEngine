#ifndef COMPONENT_UTILS_H
#define COMPONENT_UTILS_H

#include <tuple>

#include "Components.h"

//====================================================================
// Util traits for component pool data structure
//====================================================================

template<typename Tuple> struct ComponentPoolsFromTuple;
template<typename... Types>
struct ComponentPoolsFromTuple<std::tuple<Types...>>
{
	using type = std::tuple<ComponentPool<Types>...>;
};

using ComponentPools = ComponentPoolsFromTuple<Components>::type;

//====================================================================
// Util traits to validate that a given type is from the list of component
//====================================================================

template <typename Type, typename Tuple>
struct IsOneOf;

template <typename Type, typename... Types>
struct IsOneOf<Type, std::tuple<Types...>> : std::disjunction<std::is_same<Type, Types>...> {};

template <typename Type>
concept IsComponent = IsOneOf<Type, Components>::value;

#endif