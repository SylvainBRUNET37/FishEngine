#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

#include "entities/Entity.h"
#include "rendering/graphics/camera/BaseCamera.h"

struct Camera : public BaseCameraData
{
	enum class CameraMode {
		FIRST_PERSON,
		THIRD_PERSON
	};

	// 3rd person
	Entity targetEntity = INVALID_ENTITY;
	inline static float distance{};
	inline static float heightOffset{};
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
	inline static float minDistance;
	inline static float maxDistance;
	inline static float zoomSpeed;

	// Inversion des contrôles/caméra
	bool invertCamRotation = false;

	// Mode de caméra actuel
	inline static CameraMode mode = CameraMode::THIRD_PERSON;

	// Offset pour que la caméra 1ère personne soit bien placée (à ajuster potentiellement)
	inline static DirectX::XMFLOAT3 firstPersonOffset;
};
#endif

