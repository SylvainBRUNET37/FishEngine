#ifndef CONTACT_LISTENER_LOGGER_H
#define CONTACT_LISTENER_LOGGER_H

#include <iostream>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include "PhysicsEngine/systems/JoltSystem.h"

#include <Jolt/Geometry/AABox.h>

#include "Globals.h"

class ContactListenerImpl : public JPH::ContactListener
{
public:
	JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2,
	                                      JPH::RVec3Arg inBaseOffset,
	                                      const JPH::CollideShapeResult& inCollisionResult) override
	{
		//std::cout << "Contact validate callback between body1 in layer " << GetObjectLayerName(inBody1.GetObjectLayer()) << " and body2 in layer " << GetObjectLayerName(inBody2.GetObjectLayer()) << std::endl;
		if (inBody1.GetObjectLayer() == Layers::SENSOR && inBody2.GetObjectLayer() == Layers::CARGO) {
			std::cout << "nGames won was " << Globals::getNGamesWon() << std::endl;
			JPH::AABox sensorBox = inBody1.GetWorldSpaceBounds();
			JPH::AABox cargoBox = inBody2.GetWorldSpaceBounds();
			if (sensorBox.Contains(cargoBox)) {
				Globals::incrementNGamesWon();
				std::cout << "nGames won is " << Globals::getNGamesWon() << std::endl;
			}
		} else if (inBody1.GetObjectLayer() == Layers::CARGO && inBody2.GetObjectLayer() == Layers::SENSOR) {
			std::cout << "nGames won was " << Globals::getNGamesWon() << std::endl;
			JPH::AABox sensorBox = inBody2.GetWorldSpaceBounds();
			JPH::AABox cargoBox = inBody1.GetWorldSpaceBounds();
			if (sensorBox.Contains(cargoBox)) {
				Globals::incrementNGamesWon();
				std::cout << "nGames won is " << Globals::getNGamesWon() << std::endl;
			}
		}
		
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold,
	                    JPH::ContactSettings& ioSettings) override
	{
		//std::cout << "A contact was added" << '\n';
		//std::cout << "A contact was added between body1 in layer " << GetObjectLayerName(inBody1.GetObjectLayer()) << " and body2 in layer " << GetObjectLayerName(inBody2.GetObjectLayer()) << std::endl;

		//Replaces calculated mass (based on what?) with whatever's needed, here enough for a small ball to move a cube of cargo
		if (inBody1.GetObjectLayer() == Layers::BALL) {
			ioSettings.mInvMassScale1 = 0.01f; //To test pushing cargo inside goal, changed these 0.01f to 0 and use scene "sceneDevoir3_sphere.glb". These really should be constants or perhaps a function...
			ioSettings.mInvInertiaScale1 = 0.01f;
			ioSettings.mInvMassScale2 = 1.0f;
			ioSettings.mInvInertiaScale2 = 1.0f;
		} else if (inBody2.GetObjectLayer() == Layers::BALL) {
			ioSettings.mInvMassScale1 = 1.0f;
			ioSettings.mInvInertiaScale1 = 1.0f;
			ioSettings.mInvMassScale2 = 0.01f;
			ioSettings.mInvInertiaScale2 = 0.01f;
		}
	}

	void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold,
	                        JPH::ContactSettings& ioSettings) override
	{
	}

	void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
	{
		//std::cout << "A contact was removed" << '\n';
	}

private:
	//This really should be in Layers.h, but then the compiler complained it was already defined for some reason
	const char* GetObjectLayerName(JPH::ObjectLayer inLayer)
	{
		switch (inLayer)
		{
		case Layers::NON_MOVING:	return "NON_MOVING";
		case Layers::VEHICLE:		return "VEHICLE";
		case Layers::SENSOR:		return "SENSOR";
		case Layers::CARGO:			return "CARGO";
		case Layers::BALL:			return "BALL";
		default:					JPH_ASSERT(false); return "INVALID";
		}
	}
};

#endif
