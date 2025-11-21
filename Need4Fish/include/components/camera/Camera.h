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

	// Inversion des contrôles/caméra
	bool invertCamRotation = false;

	// Mode de caméra actuel
	CameraMode mode = CameraMode::THIRD_PERSON;

	// Offset pour que la caméra 1ère personne soit bien placée (à ajuster potentiellement)
	DirectX::XMFLOAT3 firstPersonOffset = { 0.0f,-1.5f,35.0f };
};
#endif

