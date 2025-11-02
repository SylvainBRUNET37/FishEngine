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

	const Transform* followedCube = nullptr;

public:
	Camera(XMVECTOR position, XMVECTOR focus, XMVECTOR up, float viewWidth, float viewHeight)
		: position(position), focus(focus), up(up),
		viewWidth(viewWidth > 0 ? viewWidth : 1),
		viewHeight(viewHeight > 0 ? viewHeight : 1)
	{
		aspectRatio = static_cast<float>(viewWidth) / static_cast<float>(viewHeight);
		matView = XMMatrixLookAtRH(position, focus, up);
		matProj = XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane);
	}

	void FollowCube(const Transform* cubeTransform) {
		followedCube = cubeTransform;
	}

	virtual ~Camera() = default;

	XMMATRIX getMatView() const noexcept { return matView; }
	XMMATRIX getMatProj() const noexcept { return matProj; }
	XMVECTOR GetPosition() const noexcept { return position; }
	XMVECTOR GetFocus() const noexcept { return focus; }
	XMVECTOR GetUp() const noexcept { return up; }

	void SetPosition(const XMVECTOR pos) noexcept {
		position = pos;
		UpdateMatView();
	}

	void SetFocus(const XMVECTOR foc) noexcept {
		focus = foc;
		UpdateMatView();
	}

	virtual void Move(float deltaForward, float deltaSide, float deltaHeight) noexcept = 0;
	virtual void Rotate(float yawDelta, float pitchDelta) noexcept = 0;

protected:
	void UpdateMatView() noexcept {
		matView = XMMatrixLookAtRH(position, focus, up);
	}
};

class FirstPersonCamera : public Camera {
	float pitchAccum = 0.0f;
	float yawAccum = 0.0f;

	void CalculateInitialAngles(XMVECTOR forward) noexcept {
		yawAccum = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));
		pitchAccum = asinf(XMVectorGetY(forward));
	}

	void UpdateOrientation() noexcept {
		constexpr float maxPitch = XM_PIDIV2 - 0.01f;
		pitchAccum = std::clamp(pitchAccum, -maxPitch, maxPitch);

		XMVECTOR forward = XMVectorSet(
			cosf(pitchAccum) * sinf(yawAccum),
			sinf(pitchAccum),
			cosf(pitchAccum) * cosf(yawAccum),
			0.0f
		);

		XMVECTOR right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward));
		up = XMVector3Normalize(XMVector3Cross(forward, right));
		focus = XMVectorAdd(position, forward);
	}

public:
	FirstPersonCamera(XMVECTOR position, XMVECTOR focus, XMVECTOR up, float viewWidth, float viewHeight)
		: Camera(position, focus, up, viewWidth, viewHeight)
	{
		XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(focus, position));
		CalculateInitialAngles(forward);
	}

	float GetYaw() const noexcept { return yawAccum; }
	float GetPitch() const noexcept { return pitchAccum; }

	void Move(float deltaForward, float deltaSide, float deltaHeight) noexcept override {
		XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(focus, position));
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));

		XMVECTOR move = XMVectorAdd(
			XMVectorScale(forward, deltaForward),
			XMVectorScale(right, deltaSide)
		);

		position = XMVectorAdd(position, move);
		focus = XMVectorAdd(focus, move);
		UpdateMatView();
	}

	void Rotate(float yawDelta, float pitchDelta) noexcept override {
		yawAccum += yawDelta;
		pitchAccum += pitchDelta;
		UpdateOrientation();
		UpdateMatView();
	}
};

class ThirdPersonCamera : public Camera {
	XMVECTOR targetPosition;
	float distance;
	float heightOffset;
	float yawAccum = 0.0f;
	float pitchAccum = 0.0f;

	static constexpr float MIN_DISTANCE = 5.0f;
	static constexpr float MIN_HORIZONTAL_DIST = 1.0f;
	static constexpr float MIN_PITCH = -XM_PIDIV4;
	static constexpr float MAX_PITCH = XM_PIDIV2 - 0.1f;
	static constexpr float PITCH_MARGIN = 0.1f;

	XMVECTOR CalculateCameraPosition() noexcept {
		float safePitch = std::clamp(pitchAccum, MIN_PITCH + PITCH_MARGIN, MAX_PITCH - PITCH_MARGIN);

		float horizontalDistance = distance * cosf(safePitch);
		float verticalDistance = distance * sinf(safePitch);
		float actualHorizontalDist = max(abs(horizontalDistance), MIN_HORIZONTAL_DIST);

		return XMVectorSet(
			XMVectorGetX(focus) - actualHorizontalDist * sinf(yawAccum),
			XMVectorGetY(focus) + verticalDistance,
			XMVectorGetZ(focus) - actualHorizontalDist * cosf(yawAccum),
			1.0f
		);
	}

	bool IsPositionValid(XMVECTOR pos) noexcept {
		XMVECTOR diff = XMVectorSubtract(focus, pos);
		return XMVectorGetX(XMVector3Length(diff)) >= 0.5f;
	}

	XMVECTOR GetFallbackPosition() noexcept {
		return XMVectorSet(
			XMVectorGetX(focus),
			XMVectorGetY(focus) + 5.0f,
			XMVectorGetZ(focus) - 10.0f,
			1.0f
		);
	}

	void CalculateUpVector() noexcept {
		XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(focus, position));
		XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);

		float dotProduct = abs(XMVectorGetX(XMVector3Dot(forward, worldUp)));
		if (dotProduct > 0.99f) {
			worldUp = XMVectorSet(0, 0, 1, 0);
		}

		XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, forward));
		up = XMVector3Normalize(XMVector3Cross(forward, right));
	}

	void UpdateCameraPosition() noexcept {
		if (followedCube) {
			targetPosition = XMLoadFloat3(&followedCube->position);
		}
		
		// Décalage pour être plus haut et plus loin
		distance = 50.f;
		heightOffset = 25.f;

		XMVECTOR heightOffsetVec = XMVectorSet(0.0f, heightOffset, -distance, 0.0f);
		focus = XMVectorAdd(targetPosition, heightOffsetVec);
		position = CalculateCameraPosition();

		if (!IsPositionValid(position)) {
			position = GetFallbackPosition();
		}

		CalculateUpVector();
		UpdateMatView();
	}

public:
	ThirdPersonCamera(XMVECTOR targetPos, float distance, float heightOffset,
		float viewWidth, float viewHeight, float initialYaw = 0.0f, float initialPitch = 0.0f)
		: Camera(XMVectorSet(0, 5, -10, 1), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0), viewWidth, viewHeight),
		targetPosition(targetPos),
		distance(max(MIN_DISTANCE, distance)),
		heightOffset(heightOffset),
		yawAccum(initialYaw),
		pitchAccum(std::clamp(initialPitch, MIN_PITCH + PITCH_MARGIN, MAX_PITCH - PITCH_MARGIN))
	{
		UpdateCameraPosition();
	}

	void SetTargetPosition(XMVECTOR targetPos) noexcept {
		targetPosition = targetPos;
		UpdateCameraPosition();
	}

	float GetYaw() const noexcept { return yawAccum; }
	float GetPitch() const noexcept { return pitchAccum; }

	void Move(float deltaForward, float deltaSide, float deltaHeight) noexcept override {
		XMVECTOR forward = XMVector3Normalize(XMVectorSet(sinf(yawAccum), 0, cosf(yawAccum), 0));
		XMVECTOR right = XMVector3Normalize(XMVectorSet(cosf(yawAccum), 0, -sinf(yawAccum), 0));

		XMVECTOR move = XMVectorAdd(
			XMVectorAdd(
				XMVectorScale(forward, deltaForward),
				XMVectorScale(right, deltaSide)
			),
			XMVectorSet(0, deltaHeight, 0, 0)
		);

		targetPosition = XMVectorAdd(targetPosition, move);
		UpdateCameraPosition();
	}

	void Rotate(float yawDelta, float pitchDelta) noexcept override {
		yawAccum += yawDelta;
		pitchAccum = std::clamp(pitchAccum + pitchDelta, MIN_PITCH, MAX_PITCH);
		UpdateCameraPosition();
	}

	void SetDistance(float newDistance) noexcept {
		distance = max(1.0f, newDistance);
		UpdateCameraPosition();
	}

	void SetHeightOffset(float newHeightOffset) noexcept {
		heightOffset = newHeightOffset;
		UpdateCameraPosition();
	}
};