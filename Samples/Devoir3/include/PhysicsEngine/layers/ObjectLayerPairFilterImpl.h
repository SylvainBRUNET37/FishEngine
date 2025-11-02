#ifndef OBJECT_LAYER_PAIR_FILTER_IMPL_H
#define OBJECT_LAYER_PAIR_FILTER_IMPL_H

#include "Layers.h"

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
	[[nodiscard]] bool ShouldCollide(const JPH::ObjectLayer inLayer1, const JPH::ObjectLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::VEHICLE:
			return inLayer2 == Layers::NON_MOVING;
		case Layers::NON_MOVING:
			return inLayer2 == Layers::VEHICLE || inLayer2 == Layers::BALL || Layers::CARGO;
		case Layers::SENSOR:
			return inLayer2 == Layers::CARGO;
		case Layers::CARGO:
			return inLayer2 == Layers::NON_MOVING || inLayer2 == Layers::BALL || inLayer2 == Layers::SENSOR;
		case Layers::BALL:
			return inLayer2 == Layers::CARGO || inLayer2 == Layers::NON_MOVING;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

#endif
