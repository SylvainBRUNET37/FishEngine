#include "pch.h"
#include "components/physics/RigidBody.h"

#include "PhysicsEngine/JoltSystem.h"

RigidBody& RigidBody::operator=(RigidBody&& other) noexcept
{
    if (this != &other)
    {
        RemoveAndDestroyBody();
        body = std::exchange(other.body, nullptr);
    }

    return *this;
}

void RigidBody::RemoveAndDestroyBody() noexcept
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
