#ifndef LAYERS_H
#define LAYERS_H

#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;	//Basic layer for environment
	static constexpr JPH::ObjectLayer MOVING = 1;		//Basic layer for perhaps bubbles or algae?
	static constexpr JPH::ObjectLayer SENSOR = 2;		//Basic layer for... we shall see
	static constexpr JPH::ObjectLayer VEHICLE = 3;		//Special case
	static constexpr JPH::ObjectLayer FOOD_CHAIN = 4;	//What can eat or be eaten but is not controlled by the player
	static constexpr JPH::ObjectLayer GOAL = 5;			//Special case
	static constexpr JPH::ObjectLayer MOVING_DECOR = 6;	//Rocks, ...
	static constexpr JPH::ObjectLayer NUM_LAYERS = 7;
};

#endif