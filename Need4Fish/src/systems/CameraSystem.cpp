#include "pch.h"
#include "systems/CameraSystem.h"
#include "GameState.h"

using namespace DirectX;

void CameraSystem::Update(double, EntityManager& entityManager)
{
	if (GameState::currentState == GameState::PLAYING)
	{
		for (const auto& [entity, camera] : entityManager.View<Camera>())
		{
			HandleRotation(camera);
			UpdateCameraMatrices(camera, entityManager);
		}
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

	// Yaw et pitch visés
	const float totalYaw = targetYaw + camera.yawOffset;
	camera.targetYaw = totalYaw; // Pour la physique	
	camera.targetPitch = camera.pitchAngle;

	if (camera.mode == Camera::CameraMode::FIRST_PERSON)
	{
		// Position à l'intérieur/devant le mosasaure
		const XMVECTOR fpOffset = XMLoadFloat3(&camera.firstPersonOffset);

		// Position selon la rotation avec l'offset
		const XMVECTOR rotatedOffset = XMVector3Transform(fpOffset, targetRotMat);
		camera.position = XMVectorAdd(targetPos, rotatedOffset);

		// On utilise directement le forward vector du mosasaure
		const XMVECTOR lookDirection = XMVector3Normalize(forward);
		camera.focus = XMVectorAdd(camera.position, XMVectorScale(lookDirection, 100.0f));
	}
	else // THIRD_PERSON
	{
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

	const auto deltaX = static_cast<float>(currentCursorCoordinates.x - cameraData.cursorCoordinates.x);
	const auto deltaY = static_cast<float>(currentCursorCoordinates.y - cameraData.cursorCoordinates.y);

	static constexpr float deadzone = 2.0f; // Pixels de tolérance

	// Appliquer la rotation seulement si le mouvement dépasse la deadzone
	if (std::abs(deltaX) > deadzone || std::abs(deltaY) > deadzone)
	{
		// Sensibilité ajustée selon le mode
		float yawSensitivity, pitchSensitivity;
		if (cameraData.mode == Camera::CameraMode::FIRST_PERSON)
		{
			yawSensitivity = 0.00005f;
			pitchSensitivity = 0.00020f;
		}
		else
		{
			yawSensitivity = 0.002f;
			pitchSensitivity = 0.002f;
		}

		// Inversion si true
		const float yawMultiplier = cameraData.invertCamRotation ? -1.0f : 1.0f;
		const float pitchMultiplier = cameraData.invertCamRotation ? 1.0f : -1.0f;

		Rotate(cameraData,
			deltaX * yawSensitivity * yawMultiplier,
			deltaY * pitchSensitivity * pitchMultiplier);
	}
	else
	{
		// Ramener progressivement vers zéro quand pas de mouvement
		if (std::abs(cameraData.yawOffset) > 0.001f)
		{
			static constexpr float returnSpeed = 0.02f;
			const float returnStep = std::copysign(returnSpeed, -cameraData.yawOffset);
			cameraData.yawOffset += returnStep;

			// Snap à zéro si très proche
			if (std::abs(cameraData.yawOffset) < returnSpeed)
			{
				cameraData.yawOffset = 0.0f;
			}
		}
	}

	// Recentrer seulement si la souris s'éloigne suffisament du centre
	static constexpr float recenterThreshold = 100.0f;
	const float distanceFromCenter = sqrtf(
		powf(currentCursorCoordinates.x - cameraData.screenCenter.x, 2.0f) +
		powf(currentCursorCoordinates.y - cameraData.screenCenter.y, 2.0f)
	);

	if (distanceFromCenter > recenterThreshold)
	{
		SetCursorPos(cameraData.screenCenter.x, cameraData.screenCenter.y);
		cameraData.cursorCoordinates = cameraData.screenCenter;
	}
	else
	{
		cameraData.cursorCoordinates = currentCursorCoordinates;
	}
}

void CameraSystem::Rotate(Camera& cameraData, const float yawDelta, const float pitchDelta)
{
	// Limiter la caméra
	constexpr float maxOffset = XM_PIDIV4; // ±45 degrés dans toutes les directions

	cameraData.yawOffset = std::clamp(
		cameraData.yawOffset + yawDelta,
		-maxOffset,
		maxOffset
	);

	cameraData.pitchAngle = std::clamp(
		cameraData.pitchAngle + pitchDelta,
		-maxOffset,
		maxOffset
	);
}

void CameraSystem::SetMouseCursor()
{
	ShowCursor(FALSE);
	Camera::isMouseCaptured = true;

	const HWND hwnd = GetActiveWindow();
	if (!hwnd)
		return;

	RECT rect;
	GetClientRect(hwnd, &rect);

	Camera::screenCenter.x = (rect.right - rect.left) / 2;
	Camera::screenCenter.y = (rect.bottom - rect.top) / 2;

	// Convertir en repère écran
	ClientToScreen(hwnd, &Camera::screenCenter);

	// Centrer le curseur
	SetCursorPos(Camera::screenCenter.x, Camera::screenCenter.y);

	// Initialiser les coordonnées de la caméra
	Camera::cursorCoordinates = Camera::screenCenter;
}
