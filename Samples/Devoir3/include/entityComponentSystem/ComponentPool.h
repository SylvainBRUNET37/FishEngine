#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <utility>
#include <vector>
#include <deque>
#include <optional>

#include "Entity.h"

// A container for the given component
template <typename Component>
class ComponentPool
{
public:
    void ResizeIfOutOfBound(const Entity::Index entityIndex)
    {
        // Expand the vector if needed
        if (components.size() < entityIndex)
        {
            components.resize(entityIndex);
        }
    }

    [[nodiscard]] bool Has(const Entity::Index entityIndex) const noexcept
    {
        return entityIndex < components.size() && components[entityIndex].has_value();
    }

    template <typename... ComponentArgs>
    Component& Emplace(const Entity::Index entityIndex, ComponentArgs&&... componentArgs)
    {
        ResizeIfOutOfBound(entityIndex + 1);

        // Construct the component with given args
        components[entityIndex].emplace(std::forward<ComponentArgs>(componentArgs)...);
        return *components[entityIndex];
    }

    void RemoveComponentOf(const Entity::Index entityIndex)
    {
        if (entityIndex < components.size())
        {
            components[entityIndex].reset(); // destroy the component if it exists
        }
    }

    [[nodiscard]] Component& Get(const Entity::Index entityIndex)
    {
        return *components[entityIndex];
    }

    [[nodiscard]] const Component& Get(const Entity::Index entityIndex) const
    {
        return *components[entityIndex];
    }

private:
    std::deque<std::optional<Component>> components;
};

#endif
