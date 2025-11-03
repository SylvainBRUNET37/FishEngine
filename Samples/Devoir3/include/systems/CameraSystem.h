#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "System.h"

class CameraSystem : public System
{
	void Update(double deltaTime, EntityManager& entityManager) override;

	static void UpdateCameraMatrices(CameraData& camera, const EntityManager& entityManager);

	static void ComputeCameraPosition(CameraData& camera, const Transform& transform);
	static void ComputeCameraOrientation(CameraData& camera);

	static void HandleRotation(CameraData& cameraData);
	static void Rotate(CameraData& cameraData, float yawDelta, float pitchDelta);
};

#endif