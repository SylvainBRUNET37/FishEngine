#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H

#include <algorithm>
#include <functional>

#include "device/RenderContext.h"
#include "shaders/ShaderBank.h"
#include "shaders/ShaderProgramDesc.h"
#include "graphics/Model.h"

class RenderingEngine
{
public:
	using MainLoopCallback = std::function<bool()>;

	explicit RenderingEngine(RenderContext* renderContext, ShaderBank&& shaderBank_, const std::initializer_list<MainLoopCallback> callbacks)
		: shaderBank{ shaderBank_ }, mainLoopCallbacks{ callbacks }, renderContext{ renderContext }
	{
		InitScene();
		InitAnimation();
	}

	void Run();
	void AddObjectToScene(Model&& model)
	{
		scene.push_back(model);
	}

private:
	ShaderBank shaderBank;
	std::vector<MainLoopCallback> mainLoopCallbacks;

	DirectX::XMMATRIX matView{};
	DirectX::XMMATRIX matProj{};
	DirectX::XMMATRIX matViewProj{};

	RenderContext* renderContext; // TODO: use unique ptr ?

	std::vector<Model> scene;

	int64_t previousTimeCount{};

	void InitAnimation();
	void InitScene();

	static void AnimeScene(double elapsedTime);
	void RenderScene();
	void UpdateScene();
};

#endif
