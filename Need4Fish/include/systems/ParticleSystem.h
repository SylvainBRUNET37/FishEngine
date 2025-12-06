#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "System.h"

class ParticleSystem : public System
{
public:
	struct ParticleZoneParams
	{
		DirectX::XMFLOAT3 centerPosition;
		DirectX::XMFLOAT3 halfExtends;
		unsigned int nbParticle;
		float particleDurationMin;
		float particleDurationMax;
		float particleSpeed;
		DirectX::XMFLOAT3 particleDirection; // must be a unit vector
		Texture billboardTexture;
		std::shared_ptr<ShaderProgram> billboardShader;
	};

	explicit ParticleSystem(ID3D11Device* device) : device{device}
	{
	}

	void AddParticleZone(EntityManager& entityManager, const ParticleZoneParams& params);
	void Reset() { particleZones.clear(); }
	void Update(double deltaTime, EntityManager& entityManager) override;

private:
	struct Zone
	{
		std::vector<Entity> particles;
		DirectX::XMFLOAT3 centerPosition;
		DirectX::XMFLOAT3 halfExtends;
		float particleDurationMin;
		float particleDurationMax;
		float particleSpeed;
		DirectX::XMFLOAT3 particleDirection; // must be a unit vector
	};

	std::vector<Zone> particleZones;
	ID3D11Device* device;

	static void MoveAndTeleportIfAtEndOfLife(EntityManager& entityManager, const Zone& zone, const Entity& entity,
	                                  double deltaTime);
};

#endif
