#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

#include "entities/Entity.h"
#include "rendering/graphics/camera/BaseCamera.h"

struct Camera : public BaseCameraData
{
	// 3rd person
	Entity targetEntity = INVALID_ENTITY;
	float distance{};
	float heightOffset{};
	float yawOffset = 0.0f;
	float pitchAngle = -0.1f;

	// Direction cible pour le poisson
	float targetYaw = 0.0f; 
	float targetPitch = 0.0f;

	// Mouse
	inline static POINT cursorCoordinates;

	// Pour limiter/capturer la souris
	inline static POINT screenCenter;
	inline static bool isMouseCaptured = false;	

	// Pour zoomer/dézoomer
	float minDistance = 50.0f;
	float maxDistance = 170.0f;
	float zoomSpeed = 1.0f;
};
#endif

