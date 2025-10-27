#ifndef OBJECT_VS_BROAD_PHASE_LAYER_FILTER_IMPL_H
#define OBJECT_VS_BROAD_PHASE_LAYER_FILTER_IMPL_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

#include "BroadPhaseLayers.h"
#include "Layers.h"

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	[[nodiscard]] bool
	ShouldCollide(const JPH::ObjectLayer objectLayer, const JPH::BroadPhaseLayer broadPhaseLayer) const override
	{
		switch (objectLayer)
		{
		case Layers::NON_MOVING:
			return broadPhaseLayer == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return broadPhaseLayer == BroadPhaseLayers::NON_MOVING;
		default:
			return false;
		}
	}
};

#endif
