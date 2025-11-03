#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

#include "entities/Entity.h"

using namespace DirectX;

struct Camera
{
	XMVECTOR position{};
	XMVECTOR focus{};
	XMVECTOR up{};

	XMMATRIX matView{};
	XMMATRIX matProj{};

	float nearPlane = 0.05f;
	float farPlane = 1000.0f;
	float fov = XM_PI / 3.0f;

	float aspectRatio = 1920.f / 1080.f;

	// 3rd person
	Entity targetEntity = INVALID_ENTITY;
	float distance{};
	float heightOffset{};
	float yawOffset = 0.0f;
	float pitchAngle = -0.1f;

	// Mouse
	POINT cursorCoordinates;
};

#endif