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
	static const auto& shaderBank = Locator::Get<ResourceManager>().GetShaderBank();

	static const ShaderProgram billboardShader
	{
		device,
		shaderBank.Get<VertexShader>("shaders/BillboardVS.hlsl"),
		shaderBank.Get<PixelShader>("shaders/BubblePS.hlsl")
	};

	static const Texture billboardTexture = TextureLoader::LoadTextureFromFile("assets/textures/bble.png", device);

	vector<Entity> particles(params.nbParticle);

	ranges::for_each(particles, [&](Entity& particle)
	{
		particle = entityManager.CreateEntity();
		entityManager.AddComponent<Particle>(particle,
		                                     MathsUtils::RandomBetween(params.particleDurationMin, params.particleDurationMax),
		                                     params.particleDurationMax);
		entityManager.AddComponent<Billboard>(particle, Billboard
		                                      {
			                                      billboardShader,
			                                      billboardTexture,
			                                      device,
			                                      {},
			                                      {0.0f, 0.0f},
			                                      Billboard::CameraFacing
		                                      });
	});

	particleZones.emplace_back
	(
		std::move(particles),
		params.centerPosition, params.halfExtends,
		params.particleDurationMin, params.particleDurationMax,
		params.particleSpeed, params.particleDirection
	);
}

void ParticleSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	if (GameState::currentState == GameState::PLAYING)
	{
		ranges::for_each(particleZones, [&](const Zone& zone)
		{
			ranges::for_each(zone.particles, [&](const Entity& entity)
			{
				MoveAndTeleportIfAtEndOfLife(entityManager, zone, entity, deltaTime);
			});
		});
	}
}

void ParticleSystem::MoveAndTeleportIfAtEndOfLife(
	EntityManager& entityManager, const Zone& zone, const Entity& entity, const double deltaTime)
{
	auto& particle = entityManager.Get<Particle>(entity);
	auto& billboard = entityManager.Get<Billboard>(entity);

	const XMVECTOR position = XMLoadFloat3(&billboard.position);
	const XMVECTOR direction = XMLoadFloat3(&zone.particleDirection);

	XMStoreFloat3(&billboard.position, XMVectorAdd(position, direction * zone.particleSpeed));

	particle.lifeTime += deltaTime;
	if (particle.lifeTime >= particle.lifeDuration) [[unlikely]]
	{
		static constexpr float scaleMin = 5.0f;
		static constexpr float scaleMax = 15.0f;

		const auto scale = MathsUtils::RandomBetween(scaleMin, scaleMax);

		billboard.position = MathsUtils::RandomPosInSquare(zone.centerPosition, zone.halfExtends);
		billboard.scale = {scale, scale};

		particle.lifeDuration = MathsUtils::RandomBetween(zone.particleDurationMin, zone.particleDurationMax);
		particle.lifeTime = 0.0f;
	}
}
