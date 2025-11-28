#include "pch.h"
#include "physicsEngine/listeners/ContactListenerImpl.h"

using namespace JPH;
using namespace std;

void ContactListenerImpl::OnContactAdded(const Body& inBody1, const Body& inBody2,
                                         const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	LogBodyContact(inBody1, inBody2);
	GameState::detectedCollisions.push({inBody1.GetID(), inBody2.GetID()});


}

void ContactListenerImpl::LogBodyContact(const Body& inBody1, const Body& inBody2)
{
	cout << "A contact was added between body1 in layer " << GetObjectLayerName(inBody1.GetObjectLayer()) <<
		" and body2 in layer " << GetObjectLayerName(inBody2.GetObjectLayer()) << endl;
	cout << "Body1 has ID=" << inBody1.GetID().GetIndexAndSequenceNumber() << " and Body2 has ID=" << inBody2.
		GetID().GetIndexAndSequenceNumber() << endl;
}
