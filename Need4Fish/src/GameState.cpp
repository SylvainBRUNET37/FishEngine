#include "pch.h"
#include "GameState.h"

void GameState::AddCollision(const std::pair<JPH::BodyID, JPH::BodyID>& collision)
{
	std::lock_guard lock(pendingMutex);
	detectedCollisions.emplace(collision);
}

std::queue<std::pair<JPH::BodyID, JPH::BodyID>>& GameState::GetCollisions()
{
	return detectedCollisions;
}

void GameState::Init()
{
	playTime = 0.0f;
	rDeltaTime = 0.0f;
	apocalipseTime = 0.0;
	isGrowing = false;
	dirLight =
	{
		.ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		.diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f),
		.specular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

		.direction = DirectX::XMFLOAT3(-0.5f, -1.0f, 0.5f),
		.pad = 0.0f
	};
	colorTint = {0, 0, 0};
	postProcessSettings = {};
	currentState = PLAYING;
	currentCameraEntity = INVALID_ENTITY;
	playerEntity = INVALID_ENTITY;
	while (!detectedCollisions.empty())
		detectedCollisions.pop();
}
