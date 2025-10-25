#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <utility>
#include <vector>

#include "Entity.h"

template <typename Component>
class ComponentPool
{
public:
	ComponentPool() = default;

	void ResizeIfOutOfBound(const Entity::Index entityIndex)
	{
		// Resize the container of components (+ presence) if their is a new entity index
		if (components.size() < entityIndex)
		{
			components.resize(entityIndex);
			present.resize(entityIndex, false);
		}
	}

	[[nodiscard]] bool Has(const Entity::Index entityIndex) const noexcept
	{
		return entityIndex < present.size() && present[entityIndex];
	}

	template <typename... ComponentArgs>
	Component& Emplace(const Entity::Index entityIndex, ComponentArgs&&... componentArgs)
	{
		ResizeIfOutOfBound(entityIndex + 1);

		// Create the component with the given args at the entity position in the container
		components[entityIndex] = Component{std::forward<ComponentArgs>(componentArgs)...};
		present[entityIndex] = true;

		return components[entityIndex];
	}

	void RemoveComponentOf(const Entity::Index entityIndex)
	{
		if (entityIndex < present.size())
			present[entityIndex] = false;
	}

	[[nodiscard]] Component& Get(const Entity::Index entityIndex) { return components[entityIndex]; }
	[[nodiscard]] const Component& Get(const Entity::Index entityIndex) const { return components[entityIndex]; }

private:
	std::vector<Component> components; // The container of component. The index is the entity which hold the component
	std::vector<bool> present; // Indicate if enties 0..n has this component
};

#endif
