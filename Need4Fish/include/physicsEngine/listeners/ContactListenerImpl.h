#ifndef CONTACT_LISTENER_LOGGER_H
#define CONTACT_LISTENER_LOGGER_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Body/Body.h>

#include "GameState.h"
#include "physicsEngine/layers/Layers.h"

class ContactListenerImpl : public JPH::ContactListener
{
public:
	JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2,
		JPH::RVec3Arg inBaseOffset,
		const JPH::CollideShapeResult& inCollisionResult) override
	{
		std::cout << "Contact validate callback between body1 in layer " << GetObjectLayerName(inBody1.GetObjectLayer()) << " and body2 in layer " << GetObjectLayerName(inBody2.GetObjectLayer()) << std::endl;
		std::cout << "Body1 has ID=" << inBody1.GetID().GetIndexAndSequenceNumber() << " and Body2 has ID=" << inBody2.GetID().GetIndexAndSequenceNumber() << std::endl;
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold,
		JPH::ContactSettings& ioSettings) override;

	void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold,
		JPH::ContactSettings& ioSettings) override
	{
	}

	void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;

private:
	//This really should be in Layers.h, but then the compiler complained it was already defined for some reason
	static const char* GetObjectLayerName(const JPH::ObjectLayer inLayer)
	{
		switch (inLayer)
		{
		case Layers::NON_MOVING:	return "NON_MOVING";
		case Layers::MOVING:		return "MOVING";
		case Layers::SENSOR:		return "SENSOR";
		case Layers::VEHICLE:		return "VEHICLE";
		case Layers::FOOD_CHAIN:	return "FOOD_CHAIN";
		case Layers::GOAL:			return "GOAL";
		default:					JPH_ASSERT(false); return "INVALID";
		}
	}

	static void LogBodyContact(const JPH::Body& inBody1, const JPH::Body& inBody2);
};



#endif