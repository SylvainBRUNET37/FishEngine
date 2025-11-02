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

	void UpdateScene(double elapsedTime);
	void Render(const Mesh& mesh, const Transform& transform);
	void Render() const { renderContext->Present(); }

private:
	static constexpr int frameCbRegisterNumber = 0; // b0
	static constexpr int objectCbRegisterNumber = 1; // b1
	Renderer renderer;

	SceneData sceneData;
	RenderContext* renderContext;


	//Nouvelles méthodes pour rendre le code plus visible
	void InitializeCamera();
	void SwitchToThirdPerson();
	void SwitchToFirstPerson();
	bool HandleCameraSwitch();
	bool HandleMovement(float deplacement, float& deltaX, float& deltaZ);
	bool HandleRotation();

	// TODO : reusinage
	float cameraSpeed = 50.0f; // Vitesse de déplacement à utiliser // A dégager
	POINT cursorCoordinates; // Structure to store the cursor's coordinates TODO: a dégager
	std::unique_ptr<Camera> displayedCamera;
	XMVECTOR oldFocus;
	bool isFirstPerson = true;
	// END TODO

	void AnimeScene(double elapsedTime);
	void RenderScene();

	static SceneData CreateSceneData();
};

#endif
