#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "Entity.h"
#include "ComponentPool.h"
#include "Components.h"

class EntityManager
{
public:
	[[nodiscard]] Entity CreateEntity()
	{
		const auto index = freeIndices.empty() ? CreateNewIndex() : UseFreeIndex();
		const auto generation = generations[index];

		return {index, generation};
	}

	void Kill(const Entity entity)
	{
		if (not IsAlive(entity))
			return;

		// Erase the enttiy from every component pool
		std::apply
		(
			[&](auto&... pools) { (pools.RemoveComponentOf(entity.index), ...); },
			componentPools
		);

		// Entity is dead so we can reuse it's index and increment the generation
		freeIndices.push_back(entity.index);
		++generations[entity.index];
	}

	[[nodiscard]] bool IsAlive(const Entity entity) const noexcept
	{
		// If the entity's geenration is the current one it means it's alichve
		return IsFromCurrentGeneration(entity);
	}

	template <typename Component, typename... ComponentArgs>
		requires OneOf<Component>
	Component& AddComponent(const Entity entity, ComponentArgs&&... args)
	{
		if (not IsAlive(entity))
			throw std::runtime_error("Invalid entity");

		auto& componentPool = std::get<ComponentPool<Component>>(componentPools);

		componentPool.ResizeIfOutOfBound(entity.index + 1);
		return componentPool.Emplace(entity.index, std::forward<ComponentArgs>(args)...);
	}

	template <typename Component>
		requires OneOf<Component>
	[[nodiscard]] bool HasComponent(const Entity entity) const noexcept
	{
		if (not IsAlive(entity))
			return false;

		const auto& componentPool = std::get<ComponentPool<Component>>(componentPools);
		return componentPool.Has(entity.index);
	}

	template <typename Component>
		requires OneOf<Component>
	[[nodiscard]] Component& Get(const Entity entity)
	{
		if (not IsAlive(entity))
			throw std::runtime_error("Invalid entity");

		auto& componentPool = std::get<ComponentPool<Component>>(componentPools);
		return componentPool.Get(entity.index);
	}

	template <typename Component>
		requires OneOf<Component>
	[[nodiscard]] const Component& Get(const Entity entity) const
	{
		if (not IsAlive(entity))
			throw std::runtime_error("Invalid entity");

		const auto& componentPool = std::get<ComponentPool<Component>>(componentPools);
		return componentPool.Get(entity.index);
	}

	template <typename... QueryComponents>
	struct ComponentView
	{
		EntityManager* registry;

		struct iterator
		{
			EntityManager* registry;
			Entity::Index currentEntityIndex;
			size_t nbEntity;

			iterator(EntityManager* reg, const Entity::Index start)
				: registry{reg}, currentEntityIndex{start}, nbEntity{reg->generations.size()}
			{
				SkipWhileNotValid();
			}

			void SkipWhileNotValid()
			{
				while (currentEntityIndex < nbEntity)
				{
					if (HasEveryQueryComponent(currentEntityIndex))
						break;

					++currentEntityIndex;
				}
			}

			[[nodiscard]] bool HasEveryQueryComponent(const Entity::Index entityIndex) const
			{
				return entityIndex < registry->generations.size() and
					(std::get<ComponentPool<QueryComponents>>(registry->componentPools).Has(entityIndex) && ...);
			}

			iterator& operator++()
			{
				++currentEntityIndex;

				SkipWhileNotValid();

				return *this;
			}

			bool operator!=(const iterator& other) const noexcept
			{
				return currentEntityIndex != other.currentEntityIndex or registry != other.registry;
			}

			auto operator*()
			{
				Entity entity
				{
					.index = currentEntityIndex,
					.generation = registry->generations[currentEntityIndex]
				};

				return std::forward_as_tuple
				(
					registry->Get<QueryComponents>(entity)...
				);
			}
		};

		[[nodiscard]] iterator begin() const { return iterator(registry, 0); }

		[[nodiscard]] iterator end() const
		{
			return iterator(registry, static_cast<Entity::Index>(registry->generations.size()));
		}
	};

	template <typename... QueryComponents>
	[[nodiscard]] ComponentView<QueryComponents...> View()
	{
		return ComponentView<QueryComponents...>{this};
	}

private:
	std::vector<uint32_t> freeIndices; // Indices of dead entities (can be reused with generation + 1)
	std::vector<uint32_t> generations; // Generation of each indices

	ComponentPools componentPools;

	[[nodiscard]] bool IsFromCurrentGeneration(const Entity entity) const
	{
		return entity.index < generations.size() and generations[entity.index] == entity.generation;
	}

	[[nodiscard]] Entity::Index UseFreeIndex()
	{
		const auto index = freeIndices.back();

		freeIndices.pop_back();

		return index;
	}

	[[nodiscard]] Entity::Index CreateNewIndex()
	{
		const auto index = static_cast<Entity::Index>(generations.size());

		generations.push_back(1); // Add the generation of the new entity

		// Resize every component pools to include the new entity
		std::apply
		(
			[&](auto&... pools) { (pools.ResizeIfOutOfBound(index + 1), ...); },
			componentPools
		);

		return index;
	}
};

#endif
