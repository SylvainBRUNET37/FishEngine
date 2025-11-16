#include "pch.h"
#include "resources/Parser.h"

#include "json_fwd.hpp"
#include "json.hpp"

using namespace std;

void Parser::Parse(const std::string& jsonString)
{
	const auto components = nlohmann::json::parse(jsonString, nullptr);

    if (components.contains("rigidBody")) 
    {
        const auto& rigidBody = components["rigidBody"];

        if (rigidBody.contains("type")) 
        {
            const auto type = rigidBody["type"].get<std::string>();
            cout << "RigidBody type: " << type << std::endl;
        }
    }
}
