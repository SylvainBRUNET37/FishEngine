#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "System.h"

class CameraSystem : public System
{
	void Update(double, EntityManager& entityManager) override;

	static void UpdateCameraMatrices(Camera& camera, const EntityManager& entityManager, float dt);

	static void ComputeCameraPosition(Camera& camera, const Transform& transform, float dt);
	static void ComputeCameraOrientation(Camera& camera);

	static void HandleRotation(Camera& cameraData);
	static void Rotate(Camera& cameraData, float yawDelta, float pitchDelta);

	static float PerformSpringArmRaycast(const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end, float maxDistance);
	static DirectX::XMVECTOR PerformSpringArm3D(const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& idealPos, float maxDistance);
	static float Lerp(float a, float b, float t);

public:
	static void SetMouseCursor();
	static void ScaleCamera(float scaleFactor);
};

#endif