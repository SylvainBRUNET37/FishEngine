#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <format>

#include "Entity.h"
#include "components/ComponentPool.h"
#include "components/ComponentUtils.h"
#include "rendering/utils/VerboseAssertion.h"

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

		// Erase the entity from every component pool
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
		requires IsComponent<Component>
	Component& AddComponent(const Entity entity, ComponentArgs&&... args)
	{
		if (not IsAlive(entity))
			throw std::runtime_error("Invalid entity");

		auto& componentPool = std::get<ComponentPool<Component>>(componentPools);

		return componentPool.Emplace(entity.index, std::forward<ComponentArgs>(args)...);
	}

	template <typename Component>
		requires IsComponent<Component>
	[[nodiscard]] bool HasComponent(const Entity entity) const noexcept
	{
		if (not IsAlive(entity))
			return false;

		const auto& componentPool = std::get<ComponentPool<Component>>(componentPools);
		return componentPool.Has(entity.index);
	}

	template <typename Component>
		requires IsComponent<Component>
	void RemoveComponent(const Entity entity)
	{
		if (not IsAlive(entity))
			return;

		auto& componentPool = std::get<ComponentPool<Component>>(componentPools);

		componentPool.RemoveComponentOf(entity.index);
	}

	template <typename Component>
		requires IsComponent<Component>
	[[nodiscard]] Component& Get(const Entity entity)
	{
		if (not IsAlive(entity) || not HasComponent<Component>(entity))
			throw std::runtime_error("Invalid entity");

		auto& componentPool = std::get<ComponentPool<Component>>(componentPools);
		return componentPool.Get(entity.index);
	}

	template <typename Component>
		requires IsComponent<Component>
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
		EntityManager* entityManager;

		struct iterator
		{
			EntityManager* entityManager;
			Entity::Index currentEntityIndex;
			size_t nbEntity;

			iterator(EntityManager* reg, const Entity::Index start)
				: entityManager{reg}, currentEntityIndex{start}, nbEntity{reg->generations.size()}
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
				return entityIndex < entityManager->generations.size() and
					(std::get<ComponentPool<QueryComponents>>(entityManager->componentPools).Has(entityIndex) && ...);
			}

			iterator& operator++()
			{
				++currentEntityIndex;

				SkipWhileNotValid();

				return *this;
			}

			bool operator!=(const iterator& other) const noexcept
			{
				return currentEntityIndex != other.currentEntityIndex or entityManager != other.entityManager;
			}

			std::tuple<Entity, QueryComponents&...> operator*()
			{
				Entity entity
				{
					.index = currentEntityIndex,
					.generation = entityManager->generations[currentEntityIndex]
				};

				// Construct a tuple with entity and it's components
				return std::tuple<Entity, QueryComponents&...>(
					entity,
					entityManager->Get<QueryComponents>(entity)...
				);
			}
		};

		[[nodiscard]] iterator begin() const { return iterator(entityManager, 0); }

		[[nodiscard]] iterator end() const
		{
			return iterator(entityManager, static_cast<Entity::Index>(entityManager->generations.size()));
		}
	};

	template <typename... QueryComponents>
	[[nodiscard]] ComponentView<QueryComponents...> View()
	{
		return ComponentView<QueryComponents...>{this};
	}

private:
	std::vector<uint32_t> freeIndices; // Indices of dead entities (can be reused with generation + 1)
	std::vector<uint32_t> generations; // Generation of each indices (is incremented if the entity is dead)

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

		generations.push_back(1); // Create a new generation for this index (1 = first gen)

		vassert(index < MAX_ENTITIES, 
			std::format("Cannot create a new index: it is above the limit of {} entities.", MAX_ENTITIES));

		return index;
	}
};

#endif
