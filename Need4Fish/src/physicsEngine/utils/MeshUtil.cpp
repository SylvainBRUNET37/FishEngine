#include "pch.h"
#include "physicsEngine/utils/MeshUtil.h"
#include "physicsEngine/utils/ConversionUtil.h"
#include <vector>

#include "rendering/core/Transform.h"

using namespace JPH;
using namespace std;

RMat44 MeshUtil::ToJolt(const Transform& transform)
{
	const Quat rotation = ToJolt(transform.rotation);
	const RVec3 position = ToJolt(transform.position);

	return RMat44::sRotationTranslation(rotation, position);
}

Vec3 MeshUtil::getApproximateSize(const Mesh& mesh)
{
	float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
	ranges::for_each(mesh.vertices, [&](const Vertex& vertex)
	{
		const DirectX::XMFLOAT3 vPosition = vertex.position;
		minX = min(minX, vPosition.x);
		maxX = max(maxX, vPosition.x);
		minY = min(minY, vPosition.y);
		maxY = max(maxY, vPosition.y);
		minZ = min(minZ, vPosition.z);
		maxZ = max(maxZ, vPosition.z);
	});

	return Vec3(maxX - minX, maxY - minY, maxZ - minZ);
}

TriangleList MeshUtil::generateMeshTriangleList(const Mesh& mesh, const Vec3& scale)
{
    const auto nIndices = mesh.indices.size();
    assert(nIndices % 3 == 0);

    TriangleList tList;
    tList.reserve(nIndices / 3);

    const vector<Vertex>* vertices = &mesh.vertices;
    const vector<UINT>* indices = &mesh.indices;

    for (size_t i = 0; i < nIndices; i += 3)
    {
        Float3 v0 = ConversionUtil::XMFloat3ToFloat3((*vertices)[(*indices)[i]].position);
        Float3 v1 = ConversionUtil::XMFloat3ToFloat3((*vertices)[(*indices)[i + 1]].position);
        Float3 v2 = ConversionUtil::XMFloat3ToFloat3((*vertices)[(*indices)[i + 2]].position);

        // Apply scale
        v0 = Float3(v0.x * scale.GetX(), v0.y * scale.GetY(), v0.z * scale.GetZ());
        v1 = Float3(v1.x * scale.GetX(), v1.y * scale.GetY(), v1.z * scale.GetZ());
        v2 = Float3(v2.x * scale.GetX(), v2.y * scale.GetY(), v2.z * scale.GetZ());

        tList.emplace_back(v0, v1, v2);
    }

    return tList;
}
