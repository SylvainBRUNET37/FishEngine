#include "pch.h"
#include "systems/CameraSystem.h"

using namespace DirectX;

void CameraSystem::Update(double deltaTime, EntityManager& entityManager)
{
	for (const auto& [entity, camera] : entityManager.View<Camera>())
	{
		HandleRotation(camera);
		UpdateCameraMatrices(camera, entityManager);
	}
}

void CameraSystem::ComputeCameraPosition(Camera& camera, const Transform& transform)
{
	const XMVECTOR targetPos = XMLoadFloat3(&transform.position);
	const XMVECTOR targetRotQuat = XMLoadFloat4(&transform.rotation);
	const XMMATRIX targetRotMat = XMMatrixRotationQuaternion(targetRotQuat);

	// Compute entity's yaw
	const XMVECTOR forward = targetRotMat.r[2];
	const float targetYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));

	// Adjust the height of the camera
	camera.focus = XMVectorAdd(targetPos, XMVectorSet(0, camera.heightOffset, 0, 0));

	const float totalYaw = targetYaw + camera.yawOffset;
	const float horizontalDist = camera.distance * cosf(camera.pitchAngle);
	const float verticalDist = camera.distance * sinf(camera.pitchAngle);

	camera.position = XMVectorSet
	(
		XMVectorGetX(camera.focus) - horizontalDist * sinf(totalYaw),
		XMVectorGetY(camera.focus) + verticalDist + camera.heightOffset,
		XMVectorGetZ(camera.focus) - horizontalDist * cosf(totalYaw),
		1.0f
	);
}

void CameraSystem::ComputeCameraOrientation(Camera& camera)
{
	const XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(camera.focus, camera.position));
	const XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);
	const XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, forward));

	camera.up = XMVector3Normalize(XMVector3Cross(forward, right));
}

void CameraSystem::UpdateCameraMatrices(Camera& camera, const EntityManager& entityManager)
{
	const auto targetTransform = entityManager.Get<Transform>(camera.targetEntity);

	ComputeCameraPosition(camera, targetTransform);
	ComputeCameraOrientation(camera);

	camera.matView = XMMatrixLookAtRH(camera.position, camera.focus, camera.up);
	camera.matProj = XMMatrixPerspectiveFovRH(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);
}

void CameraSystem::HandleRotation(Camera& cameraData)
{
	POINT currentCursorCoordinates;
	if (!GetCursorPos(&currentCursorCoordinates))
		return;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) 
	{
		const auto deltaX = static_cast<float>(currentCursorCoordinates.x - cameraData.cursorCoordinates.x);
		const auto deltaY = static_cast<float>(currentCursorCoordinates.y - cameraData.cursorCoordinates.y);

		constexpr float mouseSensitivity = 0.002f;
		Rotate(cameraData, deltaX * mouseSensitivity, -deltaY * mouseSensitivity);
	}

	cameraData.cursorCoordinates = currentCursorCoordinates;
}

void CameraSystem::Rotate(Camera& cameraData, const float yawDelta, const float pitchDelta)
{
	cameraData.yawOffset += yawDelta;
	cameraData.pitchAngle = std::clamp(cameraData.pitchAngle + pitchDelta, -XM_PIDIV2 + 0.1f, 0.0f);
}
