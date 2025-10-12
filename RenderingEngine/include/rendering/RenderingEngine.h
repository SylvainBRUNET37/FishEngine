#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H

#include <algorithm>
#include <functional>

#include "TextureManager.h"
#include "device/GraphicsDevice.h"
#include "shapes/Model.h"

class RenderingEngine
{
public:
	using MainLoopCallback = std::function<bool()>;

	explicit RenderingEngine(GraphicsDevice* device, const std::initializer_list<MainLoopCallback> callbacks)
		: mainLoopCallbacks{ callbacks }, device{ device }, textureMaanger{new TextureManager{}}
	{
		InitScene();
		InitAnimation();
	}

	void Run();

private:
	std::vector<MainLoopCallback> mainLoopCallbacks;

	DirectX::XMMATRIX matView{};
	DirectX::XMMATRIX matProj{};
	DirectX::XMMATRIX matViewProj{};

	GraphicsDevice* device; // TODO: use unique ptr
	TextureManager* textureMaanger;

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
