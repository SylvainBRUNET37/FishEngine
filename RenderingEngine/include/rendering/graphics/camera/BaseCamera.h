#ifndef BASE_CAMERA_H
#define BASE_CAMERA_H

#include <DirectXMath.h>

struct BaseCameraData
{
	inline static DirectX::XMVECTOR position{};
	DirectX::XMVECTOR focus{};
	DirectX::XMVECTOR up{};

	DirectX::XMMATRIX matView{};
	DirectX::XMMATRIX matProj{};

	float nearPlane = 0.05f;
	float farPlane = 10000.0f;
	float fov = DirectX::XM_PI / 3.0f;

	float aspectRatio = static_cast<float>(GetSystemMetrics(SM_CXSCREEN)) / static_cast<float>(GetSystemMetrics(SM_CYSCREEN));
};

#endif