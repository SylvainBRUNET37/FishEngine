#ifndef BODY_ACTIVATION_LISTENER_IMPL_H
#define BODY_ACTIVATION_LISTENER_IMPL_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

class BodyActivationListenerImpl : public JPH::BodyActivationListener
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
