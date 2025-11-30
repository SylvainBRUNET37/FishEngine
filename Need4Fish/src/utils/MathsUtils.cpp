#include "pch.h"
#include "utils/MathsUtils.h"

#include <random>

using namespace std;
using namespace DirectX;

float MathsUtils::RandomBetween(const float min, const float max)
{
	static mt19937 rng{std::random_device{}()};
	uniform_real_distribution dist(min, max);

	return dist(rng);
}

XMFLOAT3 MathsUtils::RandomPosInSquare(XMFLOAT3 squareCenterPosition, const float halfExtend)
{
	squareCenterPosition.x += RandomBetween(-halfExtend, halfExtend);
	squareCenterPosition.y += RandomBetween(-halfExtend, halfExtend);
	squareCenterPosition.z += RandomBetween(-halfExtend, halfExtend);

	return squareCenterPosition;
}
