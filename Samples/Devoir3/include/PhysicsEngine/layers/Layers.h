#ifndef LAYERS_H
#define LAYERS_H

#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;	//Terrain
	static constexpr JPH::ObjectLayer VEHICLE = 1;
	static constexpr JPH::ObjectLayer SENSOR = 2;
	static constexpr JPH::ObjectLayer CARGO = 3;
	static constexpr JPH::ObjectLayer BALL = 4;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 5;
};

#endif