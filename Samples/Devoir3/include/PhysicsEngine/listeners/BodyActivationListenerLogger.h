#ifndef BODY_ACTIVATION_LISTENER_LOGGER_H
#define BODY_ACTIVATION_LISTENER_LOGGER_H

#include "PhysicsEngine/pch.h"

#include <Jolt/Physics/Body/BodyActivationListener.h>

class BodyActivationListenerLogger : public JPH::BodyActivationListener
{
public:
	void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
	{
		std::cout << "A body got activated" << '\n';
	}

	void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
	{
		std::cout << "A body went to sleep" << '\n';
	}
};

#endif
