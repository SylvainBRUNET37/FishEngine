#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include "rendering/graphics/Mesh.h"
#include "Jolt/Jolt.h"
#include <Jolt/Math/Vec3.h>

class MeshUtil
{
public:
	static JPH::Vec3 getApproximateSize(const Mesh);
	static JPH::TriangleList generateMeshTriangleList(const Mesh);
};

#endif

