#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H

#include <algorithm>
#include <functional>

#include "Renderer.h"
#include "device/RenderContext.h"
#include "shaders/ShaderBank.h"
#include "shaders/ShaderProgramDesc.h"
#include "graphics/Model.h"
#include "graphics/Camera.h"

class RenderingEngine
{
public:
	using MainLoopCallback = std::function<bool()>;

	explicit RenderingEngine(RenderContext* renderContext, ShaderBank&& shaderBank_,
	                         const std::initializer_list<MainLoopCallback> callbacks)
		: shaderBank{std::move(shaderBank_)},
		  mainLoopCallbacks{callbacks},
		  renderer{renderContext->GetDevice(), frameCbRegisterNumber, objectCbRegisterNumber},
		  renderContext{renderContext}
	{
		InitScene();
		InitAnimation();
	}

	void Run();

	void AddObjectToScene(Model&& model)
	{
		scene.push_back(model);
	}

	void UpdateScene();


private:
	ShaderBank shaderBank;
	std::vector<MainLoopCallback> mainLoopCallbacks;

	static constexpr int frameCbRegisterNumber = 0; // b0
	static constexpr int objectCbRegisterNumber = 1; // b1
	Renderer renderer;

	XMMATRIX matView{};
	XMMATRIX matProj{};
	XMMATRIX matViewProj{};

	RenderContext* renderContext; // TODO: use unique ptr ?

	std::vector<Model> scene;

	int64_t previousTimeCount{};

	// TODO : reusinage
	float cameraSpeed = 50.0f; // Vitesse de déplacement à utiliser // A dégager
	POINT cursorCoordinates; // Structure to store the cursor's coordinates TODO: a dégager
	std::unique_ptr<Camera> firstPersonCamera;
	// END TODO

	void InitAnimation();
	void InitScene();

	void AnimeScene(double elapsedTime);
	void RenderScene();
};

#endif
