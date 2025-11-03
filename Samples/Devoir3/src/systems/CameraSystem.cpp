#include "pch.h"
#include "systems/CameraSystem.h"

void CameraSystem::Update(double deltaTime, EntityManager& entityManager)
{
	for (const auto& [entity, camera] : entityManager.View<CameraData>())
	{
		const auto targetEntityTransform = entityManager.Get<Transform>(camera.targetEntity);

		//
		camera.matView = XMMatrixLookAtRH(camera.position, camera.focus, camera.up);
		camera.matProj = XMMatrixPerspectiveFovRH(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);

		//
		const XMVECTOR targetPosition = XMLoadFloat3(&targetEntityTransform.position);
		const XMVECTOR targetRotationQuaternion = XMLoadFloat4(&targetEntityTransform.rotation);
		const XMMATRIX targetRotationMatrix = XMMatrixRotationQuaternion(targetRotationQuaternion);

		const XMVECTOR forward = targetRotationMatrix.r[2];
		const float tergetYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));

		camera.focus = XMVectorAdd(targetPosition, XMVectorSet(0, camera.heightOffset, 0, 0));

		const float totalYaw = tergetYaw + camera.yawOffset;
		const float horizontalDistance = camera.distance * cosf(camera.pitchAngle);
		const float verticalDistance = camera.distance* sinf(camera.pitchAngle);

		camera.position = XMVectorSet(
			XMVectorGetX(camera.focus) - horizontalDistance * sinf(totalYaw),
			XMVectorGetY(camera.focus) + verticalDistance + camera.heightOffset,
			XMVectorGetZ(camera.focus) - horizontalDistance * cosf(totalYaw),
			1.0f
		);

		const XMVECTOR forward2 = XMVector3Normalize(XMVectorSubtract(camera.focus, camera.position));
		const XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);
		const XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, forward2));
		camera.up = XMVector3Normalize(XMVector3Cross(forward2, right));

		// Update view matrix
		camera.matView = XMMatrixLookAtRH(camera.position, camera.focus, camera.up);
	}
}
