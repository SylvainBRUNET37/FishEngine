#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <memory>

#include "System.h"
#include "rendering/Renderer.h"
#include "rendering/core/Transform.h"
#include "rendering/device/RenderContext.h"
#include "rendering/graphics/Camera.h"
#include "rendering/graphics/Material.h"
#include "rendering/graphics/Mesh.h"

class RenderSystem : public System
{
public:
	explicit RenderSystem(RenderContext* renderContext, std::vector<Material>&& materials);

	void UpdateScene(double elapsedTime, const Transform& cubeTransform);
	void Render(const Mesh& mesh, const Transform& transform);
	void Render() const { renderContext->Present(); }

	void Update(const double deltaTime, EntityManager& entityManager) override
	{
		Transform cubeTransform;
		for (const auto& [entity, name, transform] : entityManager.View<Name, Transform>())
		{
			if (name.name == "Cube")
			{
				cubeTransform = transform;
			}
		}

		UpdateScene(deltaTime, cubeTransform);

		for (const auto& [entity, transform, mesh] : entityManager.View<Transform, Mesh>())
		{
			Render(mesh, transform);
		}

		Render();
	}

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