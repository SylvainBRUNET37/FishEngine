#ifndef LAYERS_H
#define LAYERS_H

#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer SENSOR = 2;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 3;
};

#endif