#include "pch.h"
#include "components/physics/Sensor.h"

#include "PhysicsEngine/JoltSystem.h"

Sensor::Sensor(Sensor&& other) noexcept :
	direction(other.direction),
	body(std::exchange(other.body, nullptr)),
	pushStrength(other.pushStrength),
	type(other.type)
{
}

Sensor& Sensor::operator=(Sensor&& other) noexcept
{
	if (this != &other)
	{
		RemoveAndDestroyBody();
		body = std::exchange(other.body, nullptr);
		pushStrength = other.pushStrength;
		direction = other.direction;
		type = other.type;
	}

	return *this;
}

void Sensor::RemoveAndDestroyBody() noexcept
{
	if (body)
	{
		auto& bodyInterface = JoltSystem::GetBodyInterface();
		const auto bodyID = body->GetID();
		bodyInterface.RemoveBody(bodyID);
		bodyInterface.DestroyBody(bodyID);
		body = nullptr;
	}
}
