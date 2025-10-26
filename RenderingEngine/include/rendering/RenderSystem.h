#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "Renderer.h"
#include "core/SceneData.h"
#include "device/RenderContext.h"
#include "graphics/Camera.h"

class RenderSystem
{
public:
	explicit RenderSystem(RenderContext* renderContext);

	void UpdateScene(double elapsedTime);
	void Draw(Model& model, const Transform& transform);
	void Render() const { renderContext->Present(); }

private:
	static constexpr int frameCbRegisterNumber = 0; // b0
	static constexpr int objectCbRegisterNumber = 1; // b1
	Renderer renderer;

	SceneData sceneData;
	RenderContext* renderContext;

	// TODO : reusinage
	float cameraSpeed = 50.0f; // Vitesse de déplacement à utiliser // A dégager
	POINT cursorCoordinates; // Structure to store the cursor's coordinates TODO: a dégager
	std::unique_ptr<Camera> firstPersonCamera;
	// END TODO

	void AnimeScene(double elapsedTime);
	void RenderScene();

	static SceneData CreateSceneData();
};

#endif
