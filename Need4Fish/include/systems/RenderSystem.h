#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "System.h"
#include "rendering/Renderer.h"
#include "rendering/device/RenderContext.h"
#include "rendering/graphics/Material.h"
#include "UIManager.h"

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

	void RenderUI(EntityManager& entityManager);
	void RenderPostProcesses(double deltaTime);
	void ComputeDistortionZones(EntityManager& entityManager);
	void RenderBillboards(EntityManager& entityManager, const Camera& currentCamera);
	void RenderMeshes(EntityManager& entityManager);
	void UpdatePointLights(EntityManager& entityManager);
	void UpdateFrameBuffer(double deltaTime, EntityManager& entityManager, const Camera& currentCamera);

	void Present() const { renderContext->Present(); }

	static FrameBuffer CreateDirectionnalLight();
};

#endif