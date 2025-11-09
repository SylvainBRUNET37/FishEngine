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
		case Layers::MOVING:
			return true;	//Collide with everything
		case Layers::NON_MOVING:
			return !(inLayer2 == Layers::NON_MOVING || inLayer2 == Layers::SENSOR);	//Collide with all except non-moving and sensor
		case Layers::SENSOR:
			return !(inLayer2 == Layers::NON_MOVING || inLayer2 == Layers::SENSOR);	//Collide with all except non-moving and sensor
		case Layers::VEHICLE:
			return true; //Collides with all except other vehicles, but there will only ever be one, so this works
		case Layers::FOOD_CHAIN:
			return !(inLayer2 == Layers::FOOD_CHAIN || inLayer2 == Layers::GOAL);	//Collides with all except Mommy and themselves
		case Layers::GOAL:
			return inLayer2 != Layers::FOOD_CHAIN;	//Collides with all but other members of the food chain. There is only ever one Mommy, so it's fine
		default:
			JPH_ASSERT(false);	//If we are here, something has gone wrong
			return false;
		}
	}
};

#endif
