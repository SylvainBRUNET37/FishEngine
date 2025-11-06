#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include "rendering/graphics/Mesh.h"
#include "Jolt/Jolt.h"
#include <Jolt/Math/Vec3.h>

class MeshUtils
{
public:
	static JPH::Vec3 getApproximateSize(const Mesh);
};

#endif

