#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <memory>

#include "System.h"
#include "rendering/Renderer.h"
#include "rendering/core/Transform.h"
#include "rendering/device/RenderContext.h"
#include "components/camera/Camera.h"
#include "rendering/graphics/Material.h"
#include "rendering/graphics/Mesh.h"

class RenderSystem : public System
{
public:
	explicit RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials);

	void Update(double deltaTime, EntityManager& entityManager) override;

private:
	static constexpr int frameCbRegisterNumber = 0;
	static constexpr int objectCbRegisterNumber = 1;

	Renderer renderer;
	SceneData sceneData;
	RenderContext* renderContext;

	void Render(const Mesh& mesh, const Transform& transform);
	void Present() const { renderContext->Present(); }

	void RenderScene() const;

	static SceneData InitSceneData();
};

#endif