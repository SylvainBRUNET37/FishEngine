#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "System.h"

class CameraSystem : public System
{
	void Update(double, EntityManager& entityManager) override;

	static void UpdateCameraMatrices(Camera& camera, const EntityManager& entityManager);

	static void ComputeCameraPosition(Camera& camera, const Transform& transform);
	static void ComputeCameraOrientation(Camera& camera);

	static void HandleRotation(Camera& cameraData);
	static void Rotate(Camera& cameraData, float yawDelta, float pitchDelta);

public:
	static void SetMouseCursor();
	static void ScaleCamera(float scaleFactor);
};

#endif