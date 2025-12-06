#include "pch.h"
#include "systems/ParticleSystem.h"

#include "GameState.h"
#include "Locator.h"
#include "rendering/texture/TextureLoader.h"
#include "resources/ResourceManager.h"
#include "utils/MathsUtils.h"

using namespace std;
using namespace DirectX;

void ParticleSystem::AddParticleZone(EntityManager& entityManager, const ParticleZoneParams& params)
{
	vector<Entity> particles(params.nbParticle);
	vector<float> particlesSpeed(params.nbParticle);

	size_t i = 0;
	ranges::for_each(particles, [&](Entity& particle)
	{
		particlesSpeed[i++] = MathsUtils::RandomBetween(params.particleSpeedMin, params.particleSpeedMax);
		particle = entityManager.CreateEntity();
		entityManager.AddComponent<Particle>(particle,
		                                     MathsUtils::RandomBetween(params.particleDurationMin,
		                                                               params.particleDurationMax),
		                                     params.particleDurationMax,
		                                     Billboard
		                                     {
			                                     params.billboardShader,
			                                     params.billboardTexture,
			                                     device,
			                                     XMFLOAT3{},
			                                     XMFLOAT2{0.0f, 0.0f},
			                                     Billboard::CameraFacing
		                                     }
		);
	});

	particleZones.emplace_back
	(
		std::move(particles),
		params.centerPosition, params.halfExtends,
		params.particleDurationMin, params.particleDurationMax,
		std::move(particlesSpeed), params.particleDirection
	);
}

void ParticleSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	if (GameState::currentState == GameState::PLAYING)
	{
		ranges::for_each(particleZones, [&](const Zone& zone)
		{
			size_t i = 0;
			ranges::for_each(zone.particles, [&](const Entity& entity)
			{
				MoveAndTeleportIfAtEndOfLife(entityManager, zone, entity, deltaTime, i);
				++i;
			});
		});
	}
}

void ParticleSystem::MoveAndTeleportIfAtEndOfLife(
	EntityManager& entityManager, const Zone& zone, const Entity& entity, const double deltaTime, const size_t indice)
{
	auto& particle = entityManager.Get<Particle>(entity);
	Billboard& billboard = particle.billboard;

	particle.lifeTime += deltaTime;

	const bool shouldBeTeleported =
		not MathsUtils::IsInsideAABB(billboard.position, zone.centerPosition, zone.halfExtends) ||
		particle.lifeTime >= particle.lifeDuration;

	if (shouldBeTeleported) [[unlikely]] // Teleport the particle
	{
		static constexpr float scaleMin = 3.0f;
		static constexpr float scaleMax = 15.0f;

		const auto scale = MathsUtils::RandomBetween(scaleMin, scaleMax);

		billboard.position = MathsUtils::RandomPosInSquare(zone.centerPosition, zone.halfExtends);
		billboard.scale = {scale, scale};

		particle.lifeDuration = MathsUtils::RandomBetween(zone.particleDurationMin, zone.particleDurationMax);
		particle.lifeTime = 0.0f;
	}
	else // Update particle position
	{
		const XMVECTOR position = XMLoadFloat3(&billboard.position);
		const XMVECTOR direction = XMLoadFloat3(&zone.particleDirection);

		XMStoreFloat3(&billboard.position, XMVectorAdd(position, direction * zone.particleSpeeds[indice]));
	}
}
