#pragma once
#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

class Camera
{
protected:
	XMVECTOR position;
	XMVECTOR focus;
	XMVECTOR up;

	XMMATRIX matView;
	XMMATRIX matProj;

	float nearPlane = 0.05f;
	float farPlane = 1000.0f;
	float fov = XM_PI / 4.0f;

	int viewWidth;
	int viewHeight;
	float aspectRatio;

public:
	Camera(XMVECTOR position, XMVECTOR focus, XMVECTOR up, float viewWidth, float viewHeight)
		: position(position), focus(focus), up(up),
		  viewWidth(viewWidth > 0 ? viewWidth : 1),
		  viewHeight(viewHeight > 0 ? viewHeight : 1)
	{
		aspectRatio = viewWidth / viewHeight;
		matView = XMMatrixLookAtRH(position, focus, up);
		matProj = XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane);
	}

	virtual ~Camera() = default;

	XMMATRIX getMatView() const noexcept { return matView; }
	XMMATRIX getMatProj() const noexcept { return matProj; }
	XMVECTOR GetPosition() const noexcept { return position; }
	XMVECTOR GetFocus() const noexcept { return focus; }
	XMVECTOR GetUp() const noexcept { return up; }

	void SetPosition(XMVECTOR pos) noexcept
	{
		position = pos;
		UpdateMatView();
	}

	void SetFocus(XMVECTOR foc) noexcept
	{
		focus = foc;
		UpdateMatView();
	}

	virtual void Move(float deltaForward, float deltaSide, float deltaHeight) noexcept = 0;
	virtual void Rotate(float yawDelta, float pitchDelta) noexcept = 0;

protected:
	void UpdateMatView() noexcept
	{
		matView = XMMatrixLookAtRH(position, focus, up);
	}
};

class ThirdPersonCamera : public Camera
{
	XMVECTOR targetPosition;
	float distance;
	float heightOffset;
	float yawOffset = 0.0f;
	float pitchAngle = -0.1f;

	static constexpr float MIN_DISTANCE = 10.0f;

	void UpdateCameraPosition(float cubeYaw) noexcept
	{
		focus = XMVectorAdd(targetPosition, XMVectorSet(0, heightOffset, 0, 0));

		float totalYaw = cubeYaw + yawOffset;
		float horizontalDistance = distance * cosf(pitchAngle);
		float verticalDistance = distance * sinf(pitchAngle);

		position = XMVectorSet(
			XMVectorGetX(focus) - horizontalDistance * sinf(totalYaw),
			XMVectorGetY(focus) + verticalDistance + heightOffset,
			XMVectorGetZ(focus) - horizontalDistance * cosf(totalYaw),
			1.0f
		);

		XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(focus, position));
		XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, forward));
		up = XMVector3Normalize(XMVector3Cross(forward, right));

		UpdateMatView();
	}

public:
	ThirdPersonCamera(XMVECTOR targetPos, float distance, float heightOffset, float viewWidth, float viewHeight)
		: Camera(XMVectorSet(0, 5, -10, 1), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0), viewWidth, viewHeight),
		  targetPosition(targetPos),
		  distance(max(MIN_DISTANCE, distance)),
		  heightOffset(heightOffset)
	{
		UpdateCameraPosition(0.0f);
	}

	void UpdateFromCube(const Transform* cubeTransform) noexcept
	{
		if (!cubeTransform) return;

		targetPosition = XMLoadFloat3(&cubeTransform->position);

		XMVECTOR cubeRotQuat = XMLoadFloat4(&cubeTransform->rotation);
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(cubeRotQuat);
		XMVECTOR forward = rotMatrix.r[2];
		float cubeYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));

		UpdateCameraPosition(cubeYaw);
	}

	void Move(float, float, float) noexcept override
	{
	}

	void Rotate(float yawDelta, float pitchDelta) noexcept override
	{
		yawOffset += yawDelta;
		pitchAngle = std::clamp(pitchAngle + pitchDelta, -XM_PIDIV2 + 0.1f, 0.0f);
		UpdateCameraPosition(0.0f);
	}

	void SetDistance(float newDistance) noexcept
	{
		distance = max(MIN_DISTANCE, newDistance);
	}
};
