#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H

#include <algorithm>
#include <functional>

#include "device/Device.h"
#include "shapes/Bloc.h"
#include "shapes/Objet3D.h"

class RenderingEngine
{
public:
	using MainLoopCallback = std::function<bool()>;

	explicit RenderingEngine(Device* device, const std::initializer_list<MainLoopCallback> callbacks)
		: mainLoopCallbacks{ callbacks }, device{ device }
	{
		InitScene();
		InitAnimation();
	}

	void AddObjectToScene(const DirectX::XMMATRIX& pos, const float dx, const float dy, const float dz)
	{
		scene.emplace_back(std::make_unique<CBloc>(pos, dx, dy, dz, device));
	}

	void AddObjectToScene(std::unique_ptr<CObjet3D> object)
	{
		scene.emplace_back(std::move(object));
	}

	void Run();

private:
	std::vector<MainLoopCallback> mainLoopCallbacks;

	DirectX::XMMATRIX matView{};
	DirectX::XMMATRIX matProj{};
	DirectX::XMMATRIX matViewProj{};
	Device* device;

	std::vector<std::unique_ptr<CObjet3D>> scene;

	int64_t previousTimeCount{};

	void InitAnimation();
	void InitScene();

	void AnimeScene(double elapsedTime) const;
	void RenderScene() const;
	void UpdateScene();
};

#endif
