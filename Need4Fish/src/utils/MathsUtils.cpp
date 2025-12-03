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

XMFLOAT3 MathsUtils::RandomPosInSquare(XMFLOAT3 squareCenterPosition, const DirectX::XMFLOAT3 halfExtends)
{
	squareCenterPosition.x += RandomBetween(-halfExtends.x, halfExtends.x);
	squareCenterPosition.y += RandomBetween(-halfExtends.y, halfExtends.y);
	squareCenterPosition.z += RandomBetween(-halfExtends.z, halfExtends.z);

	return squareCenterPosition;
}
