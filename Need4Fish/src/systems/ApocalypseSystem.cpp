#include "pch.h"
#include "systems/ApocalypseSystem.h"

#include "GameState.h"
#include "Locator.h"
#include "physicsEngine/JoltSystem.h"
#include "PhysicsEngine/ShapeFactory.h"
#include "resources/ResourceManager.h"
#include "utils/MathsUtils.h"

using namespace DirectX;

namespace
{
#ifdef NDEBUG
	constexpr float METEOR_SPAWN_INTERVAL = 0.05;
#else
	constexpr float METEOR_SPAWN_INTERVAL = 0.3;
#endif

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
	                                       MathsUtils::RandomBetween(APOCALYPSE_START_MIN, APOCALYPSE_START_MAX)
                                       },
                                       apocalypseTime
                                       {
	                                       MathsUtils::RandomBetween(APOCALYPSE_TIME_MIN, APOCALYPSE_TIME_MAX)
                                       },
                                       meteorSpawnTimer(0),
                                       meteorSpawnInterval(METEOR_SPAWN_INTERVAL),
                                       isApocalypse{false},
                                       hasApocalypsePlayed{false}
{
	GameState::apocalipseTime = apocalypseTime;
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

		// LIGHTS
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

		meteorSpawnTimer += deltaTime;

		if (meteorSpawnTimer >= meteorSpawnInterval)
		{
			meteorSpawnTimer -= meteorSpawnInterval;

			// Compute meteorite spawn zone
			constexpr XMFLOAT3 middleSpawnPos = {0.0, 22000.0, 0.0};
			constexpr XMFLOAT3 spawnHalfextend = {6000.0, 1000.0, 6000.0};
			const XMFLOAT3 meteoritePos = MathsUtils::RandomPosInSquare(middleSpawnPos, spawnHalfextend);

			// Random roation
			const float yaw = MathsUtils::RandomBetween(0.0f, XM_2PI);
			const float pitch = MathsUtils::RandomBetween(0.0f, XM_2PI);
			const float roll = MathsUtils::RandomBetween(0.0f, XM_2PI);

			XMVECTOR randomQuat = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
			randomQuat = XMQuaternionNormalize(randomQuat);

			XMFLOAT4 meteoriteRotation;
			XMStoreFloat4(&meteoriteRotation, randomQuat);

			// random scale
			const auto scale = MathsUtils::RandomBetween(0.3f, 5.0f);
			const XMFLOAT3 meteoriteScale =
			{
				scale,
				scale,
				scale
			};

			// ROCKS
			auto& rsManager = Locator::Get<ResourceManager>();
			const auto meteoriteMeshIndice = GameState::meteoriteMeshIndice;
			const auto metoriteEntity = entityManager.CreateEntity();
			const auto& transform = entityManager.AddComponent<Transform>(metoriteEntity, Transform
			                                                              {
				                                                              .position = meteoritePos,
				                                                              .rotation = meteoriteRotation,
				                                                              .scale = meteoriteScale
			                                                              });
			entityManager.AddComponent<MeshInstance>(metoriteEntity, meteoriteMeshIndice);
			const auto& mesh = rsManager.GetMesh(meteoriteMeshIndice);
			const auto& rigidBody = entityManager.AddComponent<RigidBody>(metoriteEntity,
			                                                        ShapeFactory::CreateCube(
				                                                        transform, mesh, Layers::MOVING_DECOR));

			static auto& bodyInterface = JoltSystem::GetBodyInterface();
			bodyInterface.SetAngularVelocity(rigidBody.body->GetID(),
			                                 JPH::Vec3
				{
												MathsUtils::RandomBetween(30.0f, 100.0f),
												MathsUtils::RandomBetween(30.0f, 100.0f),
												MathsUtils::RandomBetween(30.0f, 100.0f)
			                                 });
		}
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
