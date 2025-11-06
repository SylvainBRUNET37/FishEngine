#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "System.h"
#include "rendering/Renderer.h"
#include "rendering/core/Transform.h"
#include "rendering/device/RenderContext.h"
#include "rendering/graphics/Material.h"
#include "rendering/graphics/Mesh.h"

class RenderSystem : public System
{
public:
	explicit RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials);

	void Update(double deltaTime, EntityManager& entityManager) override;

private:
	Renderer renderer;
	FrameBuffer frameBuffer;
	RenderContext* renderContext;

	void Present() const { renderContext->Present(); }
	void RenderScene() const;

	static FrameBuffer AddDirectionLightToFrameBuffer();
};

#endif