#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <utility>
#include <array>
#include <new>

#include "Entity.h"

#ifndef NDEBUG
inline static constexpr size_t MAX_ENTITIES = 4'096;
#else
inline static constexpr size_t MAX_ENTITIES = 23'000;
#endif

// A container for the given component
// TODO: not the best way of doing it, using a sparse set would be better ?
template <typename Component>
class ComponentPool
{
public:
    ~ComponentPool() 
	{
        for (size_t i = 0; i < MAX_ENTITIES; ++i)
            if (alive[i])
                std::destroy_at(GetComponentAddressOf(i));
    }

    [[nodiscard]] bool Has(const Entity::Index entityIndex) const noexcept
    {
        return alive[entityIndex];
    }

    template <typename... ComponentArgs>
    Component& Emplace(const Entity::Index entityIndex, ComponentArgs&&... componentArgs)
    {
        // Construct the component with given args
        alive[entityIndex] = true;
        return *std::construct_at(GetComponentAddressOf(entityIndex), std::forward<ComponentArgs>(componentArgs)...);
    }

    void RemoveComponentOf(const Entity::Index entityIndex)
    {
        if (alive[entityIndex])
        {
            std::destroy_at(GetComponentAddressOf(entityIndex));
            alive[entityIndex] = false;
        }
    }

    [[nodiscard]] Component& Get(const Entity::Index entityIndex)
    {
        assert(alive[entityIndex]);

        return *GetComponentAddressOf(entityIndex);
    }

    [[nodiscard]] const Component& Get(const Entity::Index entityIndex) const
    {
        assert(alive[entityIndex]);

        return *GetComponentAddressOf(entityIndex);
    }

private:
    alignas(Component) std::byte componentStorage[MAX_ENTITIES][sizeof(Component)]{};
    std::array<bool, MAX_ENTITIES> alive{};

    Component* GetComponentAddressOf(const Entity::Index entityIndex)
    {
        return std::launder(reinterpret_cast<Component*>(componentStorage[entityIndex]));
    }

    const Component* GetComponentAddressOf(const Entity::Index entityIndex) const
    {
        return std::launder(reinterpret_cast<const Component*>(componentStorage[entityIndex]));
    }
};

#endif
