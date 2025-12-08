#ifndef APOCALYPSE_SYSTEM_H
#define APOCALYPSE_SYSTEM_H

#include "System.h"
#include "rendering/graphics/lights/DirectionalLight.h"

class ApocalypseSystem : public System
{
public:
	ApocalypseSystem();

private:
	static constexpr double APOCALYPSE_START_MIN = 2; // in second
	static constexpr double APOCALYPSE_START_MAX = 4; // in second

	static constexpr double APOCALYPSE_TIME_MIN = 15; // in second
	static constexpr double APOCALYPSE_TIME_MAX = 30; // in second

	DirectX::XMFLOAT3 baseSceneTint;
	DirectX::XMFLOAT3 apocalypseSceneTint;

	DirectionalLight baseLight;
	DirectionalLight apocalypseLight;

	double elapsedTime;
	double apocalypseElapsedTime;

	double apocalypseStart;
	double apocalypseTime;

	double meteorSpawnTimer;
	double meteorSpawnInterval;

	bool isApocalypse;
	bool hasApocalypsePlayed;

	void Update(double deltaTime, EntityManager& entityManager) override;

	static DirectionalLight CreateApocalypseLight();
};

#endif
