#ifndef MATHS_UTILS_H
#define MATHS_UTILS_H

#include <DirectXMath.h>

namespace MathsUtils
{
	float RandomBetween(float min, float max);
	DirectX::XMFLOAT3 RandomPosInSquare(DirectX::XMFLOAT3 squareCenterPosition, DirectX::XMFLOAT3 halfExtends);
}

#endif