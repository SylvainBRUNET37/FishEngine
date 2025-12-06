#ifndef CONVERSION_UTILS_H
#define CONVERSION_UTILS_H

#include "Jolt/Jolt.h"
#include "Jolt/Math/Vec3.h"
#include "DirectXMath.h"

class ConversionUtil
{
public:
	static JPH::Float3 XMFloat3ToFloat3(DirectX::XMFLOAT3 f) {
		return JPH::Float3(f.x, f.y, f.z);
	}

	static DirectX::XMFLOAT3 Float3ToXMFloat3(JPH::Float3 f) {
		return DirectX::XMFLOAT3(f.x, f.y, f.z);
	}

	static JPH::Vec3Arg XMFloat3ToVec3Arg(DirectX::XMFLOAT3 f) {
		return JPH::Vec3Arg(f.x, f.y, f.z);
	}

	static DirectX::XMFLOAT3 Vec3ArgToXMFloat3(JPH::Vec3Arg f) {
		return DirectX::XMFLOAT3(f.GetX(), f.GetY(), f.GetZ());
	}
};

#endif

