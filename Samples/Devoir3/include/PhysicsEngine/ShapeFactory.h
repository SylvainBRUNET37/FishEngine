#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

namespace ShapeFactory
{
	[[nodiscard]] JPH::Body* CreateCube();
	[[nodiscard]] JPH::Body* CreatePlane();
}

#endif