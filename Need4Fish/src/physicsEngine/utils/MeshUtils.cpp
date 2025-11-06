#include "pch.h"
#include "physicsEngine/utils/MeshUtils.h"

JPH::Vec3 MeshUtils::getApproximateSize(const Mesh mesh)
{
	float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
	std::for_each(mesh.vertices.begin(), mesh.vertices.end(), [&](Vertex vertex) {
			DirectX::XMFLOAT3 vPosition = vertex.position;
			minX = min(minX, vPosition.x);
			maxX = max(maxX, vPosition.x);
			minY = min(minY, vPosition.y);
			maxY = max(maxY, vPosition.y);
			minZ = min(minZ, vPosition.z);
			maxZ = max(maxZ, vPosition.z);
		});
	
	return JPH::Vec3( maxX - minX, maxY - minY, maxZ - minZ );
}
