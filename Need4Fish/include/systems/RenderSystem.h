#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <vector>

#include "System.h"
#include "rendering/Renderer.h"
#include "rendering/device/RenderContext.h"
#include "rendering/graphics/Material.h"
#include "rendering/graphics/ShadowMap.h" //Move this?
#include "UIManager.h"
#include <memory>

class RenderSystem : public System
{
public:
	explicit RenderSystem(RenderContext* renderContext, const std::shared_ptr<UIManager>& uiManager, std::vector<Material>&& materials);

	void Update(double deltaTime, EntityManager& entityManager) override;

private:
	std::shared_ptr<UIManager> uiManager;
	Renderer renderer;
	FrameBuffer frameBuffer;
	RenderContext* renderContext;

	std::vector<BillboardData> particleData;

	void RenderUI(EntityManager& entityManager);
	void RenderPostProcesses(double deltaTime, const Camera& currentCamera);
	void ComputeDistortionZones(EntityManager& entityManager);
	void RenderBillboards(EntityManager& entityManager, const Camera& currentCamera);
	void RenderParticles(EntityManager& entityManager, const Camera& currentCamera);
	void RenderMeshes(EntityManager& entityManager);
	void RenderMeshesToShadowMap(EntityManager& entityManager);
	void UpdatePointLights(EntityManager& entityManager);
	void UpdateFrameBuffer(double deltaTime, EntityManager& entityManager, const Camera& currentCamera);
	void BuildShadowTransform();
	void DrawSceneToShadowMap(EntityManager& entityManager);

	void Present() const { renderContext->Present(); }

	static FrameBuffer CreateDirectionalLight();

	static const int SHADOW_MAP_SIZE = 4096; //shadow map size must also be defined in LightningPS.hlsl
	//4096 seems to be the maximum before a performance hit
	std::unique_ptr<ShadowMap> shadowMap; //mSmap
	DirectX::XMFLOAT4X4 lightView; //mLightView
	DirectX::XMFLOAT4X4 lightProj; //mLightProj
	DirectX::XMFLOAT4X4 shadowTransform; //mShadowTransform

	struct BoundingSphere {
		BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
		DirectX::XMFLOAT3 Center;
		float Radius;
	};

	BoundingSphere sceneBoundaries;
};

#endif