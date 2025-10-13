#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H

#include <algorithm>
#include <functional>

#include "device/GraphicsDevice.h"
#include "shaders/ShaderBank.h"
#include "shaders/ShaderProgramDesc.h"
#include "shapes/Model.h"

class RenderingEngine
{
public:
	using MainLoopCallback = std::function<bool()>;

	explicit RenderingEngine(GraphicsDevice* device, ShaderBank&& shaderBank_, const std::initializer_list<MainLoopCallback> callbacks)
		: shaderBank{ shaderBank_ }, mainLoopCallbacks{ callbacks }, device{ device }
	{
		InitScene();
		InitAnimation();
	}

	void Run();

private:
	ShaderBank shaderBank;
	std::vector<MainLoopCallback> mainLoopCallbacks;

	DirectX::XMMATRIX matView{};
	DirectX::XMMATRIX matProj{};
	DirectX::XMMATRIX matViewProj{};

	GraphicsDevice* device; // TODO: use unique ptr

	std::vector<Model> scene;

	int64_t previousTimeCount{};

	void InitAnimation();
	void InitScene();
	void InitObjects();

	void AnimeScene(double elapsedTime) const;
	void RenderScene();
	void UpdateScene();
};

#endif
