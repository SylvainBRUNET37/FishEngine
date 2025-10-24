#pragma once
#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

class Camera
{
	// Vectors
	XMVECTOR position;
	XMVECTOR focus;
	XMVECTOR up;

	// Matrices
	XMMATRIX matView;
	XMMATRIX matProj;

	// Planes(walker)
	float nearPlane = 0.05;
	float farPlane = 1000.0;

	// Other
	float fov = XM_PI / 4.0f;
	int viewWidth;
	int viewHeight;
	float aspectRatio;

	// For camera rotation
	float pitchAccum = 0.0f;
	float yawAccum = 0.0f;

public:
	Camera(XMVECTOR position, XMVECTOR focus, XMVECTOR up, float viewWidth, float viewHeight)
		: position(position),
		focus(focus),
		up(up),
		viewWidth((viewWidth > 0) ? viewWidth : 1),
		viewHeight((viewHeight > 0) ? viewHeight : 1)
	{
		this->aspectRatio = viewWidth / viewHeight;

		this->matView = XMMatrixLookAtLH(
			position,
			focus,
			up
		);

		this->matProj = XMMatrixPerspectiveFovLH(
			this->fov,
			this->aspectRatio,
			this->nearPlane,
			this->farPlane
		);
	}

	XMMATRIX getMatView() const  noexcept {
		return this->matView;
	}

	XMMATRIX getMatProj() const  noexcept {
		return this->matProj;
	}


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

	void Move(float deltaX, float deltaZ) noexcept {
		const XMVECTOR delta = XMVectorSet(deltaX, 0.0f, deltaZ, 0.0f);
		position = XMVectorAdd(position, delta);
		focus = XMVectorAdd(focus, delta);
		UpdateMatView();
	}

	void MoveFirstPerson(float deltaForward, float deltaSide) noexcept {
		const XMVECTOR forward = XMVectorSubtract(focus, position);
		const XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));

		// Compute movement delta
		const XMVECTOR move = XMVectorAdd(
			XMVectorScale(forward, deltaForward),
			XMVectorScale(right, deltaSide)
		);

		// Update position, focus and viewMatrix
		position = XMVectorAdd(position, move);
		focus = XMVectorAdd(focus, move);
		UpdateMatView();
	}

	void RotateFirstPerson(float yawDelta, float pitchDelta) noexcept {
		constexpr float maxPitch = XM_PIDIV2 - 0.01f;
		yawAccum += yawDelta;
		pitchAccum += pitchDelta;

		pitchAccum = std::clamp(pitchAccum, -maxPitch, maxPitch);

		XMVECTOR forward = XMVectorSet(
			cosf(pitchAccum) * sinf(yawAccum),
			sinf(pitchAccum),
			cosf(pitchAccum) * cosf(yawAccum),
			0.0f
		);

		XMVECTOR right = XMVector3Normalize(XMVector3Cross({ 0,1,0,0 }, forward));
		up = XMVector3Normalize(XMVector3Cross(forward, right));

		focus = XMVectorAdd(position, forward);
		UpdateMatView();
	}

private:
	void UpdateMatView() noexcept {
		matView = XMMatrixLookAtLH(position, focus, up);
	}
};
