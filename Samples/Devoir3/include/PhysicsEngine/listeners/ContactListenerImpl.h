#ifndef CONTACT_LISTENER_LOGGER_H
#define CONTACT_LISTENER_LOGGER_H

#include <iostream>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include "PhysicsEngine/systems/JoltSystem.h"

class ContactListenerImpl : public JPH::ContactListener
{
public:
	JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2,
	                                      JPH::RVec3Arg inBaseOffset,
	                                      const JPH::CollideShapeResult& inCollisionResult) override
	{
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold,
	                    JPH::ContactSettings& ioSettings) override
	{
		std::cout << "A contact was added" << '\n';

	}

	void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold,
	                        JPH::ContactSettings& ioSettings) override
	{
	}

	void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
	{
		std::cout << "A contact was removed" << '\n';
	}

private:

};

#endif
