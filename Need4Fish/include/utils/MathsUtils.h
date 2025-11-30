#ifndef MATHS_UTILS_H
#define MATHS_UTILS_H

#include <DirectXMath.h>

namespace MathsUtils
{
	float RandomBetween(float min, float max);
	DirectX::XMFLOAT3 RandomPosInSquare(DirectX::XMFLOAT3 squareCenterPosition, float halfExtend);
}

#endif