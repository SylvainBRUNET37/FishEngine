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

	// Yaw actuel
	const XMVECTOR forward = targetRotMat.r[2];
	const float targetYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));

	// Yaw visé
	const float totalYaw = targetYaw + camera.yawOffset;
	camera.targetYaw = totalYaw;  // Pour la physique	

	camera.focus = XMVectorAdd(targetPos, XMVectorSet(0, camera.heightOffset, 0, 0));

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
	/*//Version avec clic
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

	cameraData.cursorCoordinates = currentCursorCoordinates;*/
	// Touche ESC pour libérer la souris
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		if (cameraData.isMouseCaptured)
		{
			ShowCursor(TRUE);
			cameraData.isMouseCaptured = false;
		}
		return;
	}

	// Clic pour recapturer la souris
	if (!cameraData.isMouseCaptured && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		cameraData.isMouseCaptured = false;
	}

	// Capturer la souris
	if (!cameraData.isMouseCaptured)
	{
		// Obtenir le centre de l'écran
		HWND hwnd = GetActiveWindow();
		if (hwnd)
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			cameraData.screenCenter.x = (rect.right - rect.left) / 2;
			cameraData.screenCenter.y = (rect.bottom - rect.top) / 2;

			// Convertir en repère écran
			ClientToScreen(hwnd, &cameraData.screenCenter);

			// Positionner la souris au centre
			SetCursorPos(cameraData.screenCenter.x, cameraData.screenCenter.y);
			cameraData.cursorCoordinates = cameraData.screenCenter;

			// Cacher le curseur -> peut-être à changer pour menu pause
			ShowCursor(FALSE);

			cameraData.isMouseCaptured = true;
		}
	}

	POINT currentCursorCoordinates;
	if (!GetCursorPos(&currentCursorCoordinates))
		return;

	const auto deltaX = static_cast<float>(currentCursorCoordinates.x - cameraData.cursorCoordinates.x);
	const auto deltaY = static_cast<float>(currentCursorCoordinates.y - cameraData.cursorCoordinates.y);

	constexpr float mouseSensitivity = 0.002f;
	constexpr float deadzone = 2.0f;  // Pixels de tolérance

	// Appliquer la rotation seulement si le mouvement dépasse la deadzone
	if (std::abs(deltaX) > deadzone || std::abs(deltaY) > deadzone)
	{
		Rotate(cameraData, deltaX * mouseSensitivity, -deltaY * mouseSensitivity);
	}
	else
	{
		// Ramener progressivement vers zéro quand pas de mouvement
		constexpr float returnSpeed = 0.02f;
		if (std::abs(cameraData.yawOffset) > 0.001f)
		{
			const float returnStep = std::copysign(returnSpeed, -cameraData.yawOffset);
			cameraData.yawOffset += returnStep;

			// Snap à zéro si très proche
			if (std::abs(cameraData.yawOffset) < returnSpeed)
			{
				cameraData.yawOffset = 0.0f;
			}
		}
	}

	// Recentrer pour rotation infinie
	SetCursorPos(cameraData.screenCenter.x, cameraData.screenCenter.y);

	cameraData.cursorCoordinates = currentCursorCoordinates;
}

void CameraSystem::Rotate(Camera& cameraData, const float yawDelta, const float pitchDelta)
{
	/*//Version avec clic
	cameraData.yawOffset += yawDelta;
	cameraData.pitchAngle = std::clamp(cameraData.pitchAngle + pitchDelta, -XM_PIDIV2 + 0.1f, 0.0f);
	*/
	// Limiter la caméra
	constexpr float maxYawOffset = XM_PIDIV4;      // ±45 degrés à gauche et à droite
	constexpr float minPitch = -XM_PIDIV4;         // -45 degrés vers le bas
	constexpr float maxPitch = XM_PIDIV4 * 0.5f;   // +22.5 degrés vers le haut... pourrait être 45 aussi

	cameraData.yawOffset = std::clamp(
		cameraData.yawOffset + yawDelta,
		-maxYawOffset,
		maxYawOffset
	);

	cameraData.pitchAngle = std::clamp(
		cameraData.pitchAngle + pitchDelta,
		minPitch,
		maxPitch
	);
}
