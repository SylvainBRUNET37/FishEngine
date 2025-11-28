#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include "rendering/graphics/Mesh.h"
#include "Jolt/Jolt.h"
#include <Jolt/Math/Vec3.h>

struct Transform;

namespace MeshUtil
{
    inline JPH::RVec3 ToJolt(const DirectX::XMFLOAT3& vector3)
    {
        return JPH::RVec3(vector3.x, vector3.y, vector3.z);
    }

    inline JPH::Quat ToJolt(const DirectX::XMFLOAT4& quaternion)
    {
        return JPH::Quat(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    }

    JPH::RMat44 ToJolt(const Transform& transform);
	JPH::Vec3 getApproximateSize(const Mesh&);
	JPH::TriangleList generateMeshTriangleList(Mesh);
}

#endif

