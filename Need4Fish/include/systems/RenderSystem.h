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
	explicit RenderSystem(RenderContext* renderContext, std::shared_ptr<UIManager> uiManager, std::vector<Material>&& materials);

	void Update(double deltaTime, EntityManager& entityManager) override;

private:

	std::shared_ptr<UIManager> uiManager;
	Renderer renderer;
	FrameBuffer frameBuffer;
	RenderContext* renderContext;

	void Present() const { renderContext->Present(); }

	static FrameBuffer CreateDirectionnalLight();
};

#endif