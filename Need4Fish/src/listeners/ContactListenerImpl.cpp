#include "pch.h"
#include "physicsEngine/listeners/ContactListenerImpl.h"

#include "systems/SensorSystem.h"
#include "physicsEngine/JoltSystem.h"

using namespace JPH;
using namespace std;

void ContactListenerImpl::OnContactAdded(const Body& inBody1, const Body& inBody2,
                                         const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	LogBodyContact(inBody1, inBody2);
	GameState::detectedCollisions.push({inBody1.GetID(), inBody2.GetID()});

	if (not inBody1.IsSensor() && not inBody2.IsSensor())
		return;

	// If a contact happen with a sensor, register it to the sensor system
	// The sensor is the of the pair
	if (inBody1.IsSensor())
		SensorSystem::AddContact({inBody1.GetID(), inBody2.GetID()});
	else
		SensorSystem::AddContact({inBody2.GetID(), inBody1.GetID()});
}

void ContactListenerImpl::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
	// Remove contact if one of the body is a sensor
	SensorSystem::RemoveContact({ inSubShapePair.GetBody1ID(), inSubShapePair.GetBody2ID() });
}

void ContactListenerImpl::LogBodyContact(const Body& inBody1, const Body& inBody2)
{
	cout << "A contact was added between body1 in layer " << GetObjectLayerName(inBody1.GetObjectLayer()) <<
		" and body2 in layer " << GetObjectLayerName(inBody2.GetObjectLayer()) << endl;
	cout << "Body1 has ID=" << inBody1.GetID().GetIndexAndSequenceNumber() << " and Body2 has ID=" << inBody2.
		GetID().GetIndexAndSequenceNumber() << endl;
}
