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

TriangleList MeshUtil::generateMeshTriangleList(const Mesh& mesh)
{
	const auto nIndices = mesh.indices.size();
	assert(nIndices % 3 == 0); //If it doesn't, we won't be able to make triangles... TODO: Do this more elegantly?
	TriangleList tList;

	const vector<Vertex>* vertices = &mesh.vertices;
	const vector<UINT>* indices = &mesh.indices;
	for (auto i = 0; i < nIndices; i += 3)
	{
		tList.push_back(Triangle(
				ConversionUtil::XMFloat3ToFloat3((*vertices)[(*indices)[i]].position),
				ConversionUtil::XMFloat3ToFloat3((*vertices)[(*indices)[i + 1]].position),
				ConversionUtil::XMFloat3ToFloat3((*vertices)[(*indices)[i + 2]].position))
		);
	}

	return tList;
}
