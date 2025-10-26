#ifndef OBJECT_LAYER_PAIR_FILTER_IMPL_H
#define OBJECT_LAYER_PAIR_FILTER_IMPL_H

#include "PhysicsEngine/pch.h"
#include "Layers.h"

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
	[[nodiscard]] bool ShouldCollide(const JPH::ObjectLayer inLayer1, const JPH::ObjectLayer inLayer2) const override
	{
		if (inLayer1 == Layers::MOVING && inLayer2 == Layers::NON_MOVING)
			return true;
		if (inLayer2 == Layers::MOVING && inLayer1 == Layers::NON_MOVING)
			return true;

		return false;
	}
};

#endif
