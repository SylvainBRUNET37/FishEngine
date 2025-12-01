#include "pch.h"
#include "systems/ParticleSystem.h"

#include "Locator.h"
#include "rendering/texture/TextureLoader.h"
#include "resources/ResourceManager.h"
#include "utils/MathsUtils.h"

using namespace std;

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
		entityManager.AddComponent<LifeSpan>(particle, params.particleDurationMax, params.particleDurationMax);
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

	particleZones.emplace_back(
		std::move(particles), 
		params.centerPosition, params.halfExtend, 
		params.particleDurationMin, params.particleDurationMax,
		params.particleSpeed
	);
}

void ParticleSystem::Update(const double deltaTime, EntityManager& entityManager)
{
	ranges::for_each(particleZones, [&](const Zone& zone)
	{
		ranges::for_each(zone.particles, [&](const Entity& entity)
		{
			TeleportIfAtEndOfLife(entityManager, zone, entity, deltaTime);
		});
	});
}

void ParticleSystem::TeleportIfAtEndOfLife(
	EntityManager& entityManager, const Zone& zone, const Entity& entity, const double deltaTime)
{
	auto& lifeSpan = entityManager.Get<LifeSpan>(entity);
	auto& billboard = entityManager.Get<Billboard>(entity);

	billboard.position.y += zone.particleSpeed * deltaTime;

	lifeSpan.lifeTime += deltaTime;
	if (lifeSpan.lifeTime >= lifeSpan.lifeDuration) [[unlikely]]
	{
		static constexpr float scaleMin = 5.0f;
		static constexpr float scaleMax = 15.0f;
		
		const auto scale = MathsUtils::RandomBetween(scaleMin, scaleMax);

		billboard.position = MathsUtils::RandomPosInSquare(zone.centerPosition, zone.halfExtend);
		billboard.scale = {scale, scale};

		lifeSpan.lifeDuration = MathsUtils::RandomBetween(zone.particleDurationMin, zone.particleDurationMax);
		lifeSpan.lifeTime = 0.0f;
	}
}
