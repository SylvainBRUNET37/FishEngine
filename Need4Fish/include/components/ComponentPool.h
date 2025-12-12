#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <utility>
#include <array>
#include <optional>

#include "Entity.h"

// TODO: not the best way of doing it, using a sparse set would be better
#ifndef NDEBUG
inline static constexpr size_t MAX_ENTITIES = 4'096;
#else
inline static constexpr size_t MAX_ENTITIES = 23'000;
#endif


// A container for the given component
template <typename Component>
class ComponentPool
{
public:
    [[nodiscard]] bool Has(const Entity::Index entityIndex) const noexcept
    {
        return entityIndex < components.size() && components[entityIndex].has_value();
    }

    template <typename... ComponentArgs>
    Component& Emplace(const Entity::Index entityIndex, ComponentArgs&&... componentArgs)
    {
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
    std::array<std::optional<Component>, MAX_ENTITIES> components;
};

#endif
