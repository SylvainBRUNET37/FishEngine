#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

#include "entities/Entity.h"

struct Camera
{
	DirectX::XMVECTOR position{};
	DirectX::XMVECTOR focus{};
	DirectX::XMVECTOR up{};

	DirectX::XMMATRIX matView{};
	DirectX::XMMATRIX matProj{};

	float nearPlane = 0.05f;
	float farPlane = 1000.0f;
	float fov = DirectX::XM_PI / 3.0f;

	float aspectRatio = 1920.f / 1080.f;

	// 3rd person
	Entity targetEntity = INVALID_ENTITY;
	float distance{};
	float heightOffset{};
	float yawOffset = 0.0f;
	float pitchAngle = -0.1f;

	// Direction cible pour le poisson
	float targetYaw = 0.0f; 

	// Mouse
	inline static POINT cursorCoordinates;

	// Pour limiter/capturer la souris
	inline static POINT screenCenter;
	inline static bool isMouseCaptured = false;
};
#endif

