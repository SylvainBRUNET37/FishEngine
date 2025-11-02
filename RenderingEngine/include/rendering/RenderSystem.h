#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <memory>
#include "Renderer.h"
#include "core/SceneData.h"
#include "device/RenderContext.h"
#include "graphics/Camera.h"

class RenderSystem
{
public:
	explicit RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials);

	void UpdateScene(double elapsedTime, const Transform& cubeTransform);
	void Render(const Mesh& mesh, const Transform& transform);
	void Render() const { renderContext->Present(); }

private:
	static constexpr int frameCbRegisterNumber = 0;
	static constexpr int objectCbRegisterNumber = 1;

	Renderer renderer;
	SceneData sceneData;
	RenderContext* renderContext;
	std::unique_ptr<ThirdPersonCamera> camera;
	POINT cursorCoordinates;

	void InitializeCamera();
	bool HandleRotation();
	void UpdateCamera(const Transform& cubeTransform);
	void RenderScene();

	static SceneData CreateSceneData();
};

#endif