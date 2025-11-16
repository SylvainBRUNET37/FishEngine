#include "pch.h"
#include "resources/Parser.h"

#include "json.hpp"

using namespace std;

void Parser::Parse(const std::string& componentData, EntityManager& entityManager, const Entity& entity)
{
    const auto components = nlohmann::json::parse(componentData, nullptr);

    for (const auto& [componentKey, componentData] : components.items())
    {
        const auto factoryIt = ranges::find_if
    	(
            componentFactoryMethods,
            [&](const auto& entry)
            {
                return entry.first == componentKey;
            }
        );

        if (factoryIt != componentFactoryMethods.end())
        {
            const auto& factory = factoryIt->second;
            factory(componentData, entityManager, entity);
        }
    }
}
