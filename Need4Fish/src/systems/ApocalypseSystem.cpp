#include "pch.h"
#include "systems/ApocalypseSystem.h"

#include "GameState.h"
#include "utils/MathsUtils.h"

using namespace DirectX;

namespace
{
	float Lerp(const float a, const float b, const float t)
	{
		return a + (b - a) * t;
	}

	XMFLOAT3 Lerp(const XMFLOAT3& a,
	              const XMFLOAT3& b,
	              const float t)
	{
		return
		{
			Lerp(a.x, b.x, t),
			Lerp(a.y, b.y, t),
			Lerp(a.z, b.z, t)
		};
	}

	XMFLOAT4 Lerp(const XMFLOAT4& a,
	              const XMFLOAT4& b,
	              const float t)
	{
		return
		{
			Lerp(a.x, b.x, t),
			Lerp(a.y, b.y, t),
			Lerp(a.z, b.z, t),
			Lerp(a.w, b.w, t)
		};
	}
}

ApocalypseSystem::ApocalypseSystem() : baseSceneTint{},
                                       apocalypseSceneTint{0.12f, 0.02f, 0.0f},
                                       baseLight{GameState::dirLight},
                                       apocalypseLight{CreateApocalypseLight()},
                                       elapsedTime{0},
                                       apocalypseElapsedTime{0},
                                       apocalypseStart
                                       {
	                                       MathsUtils::RandomBetween(APOCALYPSE_START_MIN, APOCALYPSE_TIME_MAX)
                                       },
                                       apocalypseTime
                                       {
	                                       MathsUtils::RandomBetween(APOCALYPSE_TIME_MIN, APOCALYPSE_TIME_MAX)
                                       },
                                       isApocalypse{false},
                                       hasApocalypsePlayed{false}
{
}

void ApocalypseSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	elapsedTime += deltaTime;

	// Start the apocalypse if the time has been reached
	if (elapsedTime >= apocalypseStart && !hasApocalypsePlayed)
	{
		isApocalypse = true;
		hasApocalypsePlayed = true;
	}

	// !!! A P O C A L Y P S E !!!
	if (isApocalypse)
	{
		apocalypseElapsedTime += deltaTime;

		if (apocalypseElapsedTime >= apocalypseTime)
		{
			apocalypseElapsedTime = apocalypseTime;
			isApocalypse = false;
		}

		float time = static_cast<float>(apocalypseElapsedTime / apocalypseTime);
		time = std::clamp(time, 0.0f, 1.0f); // clamp to avoid problems

		DirectionalLight current{};
		current.ambient = Lerp(baseLight.ambient, apocalypseLight.ambient, time);
		current.diffuse = Lerp(baseLight.diffuse, apocalypseLight.diffuse, time);
		current.specular = Lerp(baseLight.specular, apocalypseLight.specular, time);
		current.direction = Lerp(baseLight.direction, apocalypseLight.direction, time);

		GameState::colorTint = Lerp
		(
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			apocalypseSceneTint,
			time
		);

		GameState::dirLight = current;
	}
}

DirectionalLight ApocalypseSystem::CreateApocalypseLight()
{
	DirectionalLight apocalypseLight{};

	apocalypseLight.ambient = {0.05f, 0.02f, 0.01f, 1.0f};
	apocalypseLight.diffuse = {1.2f, 0.3f, 0.1f, 1.0f};
	apocalypseLight.specular = {0.2f, 0.1f, 0.05f, 1.0f};
	apocalypseLight.direction = {0.2f, -0.3f, -0.9f};

	return apocalypseLight;
}
