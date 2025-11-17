#include "pch.h"
#include "resources/ComponentLoader.h"

#include "json.hpp"

using namespace std;

void ComponentLoader::LoadComponent(const std::string& componentData, EntityManager& entityManager,
                                    const Entity& entity)
{
	const auto components = nlohmann::json::parse(componentData, nullptr);

	// Retrieve the component name and call the corresponding factory method
	ranges::for_each(components.items(),
	                 [&](const auto& componentPair)
	                 {
		                 const auto& [componentKey, componentData] = componentPair;
		                 const auto factory = ranges::find_if
		                 (
			                 componentFactoryMethods,
			                 [&](const auto& entry) { return entry.first == componentKey; }
		                 );

		                 if (factory != componentFactoryMethods.end())
			                 factory->second(componentData, entityManager, entity);
	                 });
}
