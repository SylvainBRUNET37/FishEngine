#include "pch.h"
#include "systems/CameraSystem.h"
#include "GameState.h"
#include <physicsEngine/JoltSystem.h>

// Pour le SpringArm
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>

#undef min
#undef max

using namespace DirectX;

void CameraSystem::Update(double dt, EntityManager& entityManager)
{
	if (GameState::currentState == GameState::PLAYING)
	{
		for (const auto& [entity, camera] : entityManager.View<Camera>())
		{
			HandleRotation(camera);
			UpdateCameraMatrices(camera, entityManager, static_cast<float>(dt));
		}
		//Terrible hack to synch skybox position to camera position
		for (const auto& [entity, transform, name] : entityManager.View<Transform, Name>())
		{
			if (name.name == "Skybox") {
				XMStoreFloat3(&transform.position, Camera::position);
			}
		}
	}
}

void CameraSystem::ComputeCameraPosition(Camera& camera, const Transform& transform, float dt)
{
	const XMVECTOR targetPos = XMLoadFloat3(&transform.position);
	const XMVECTOR targetRotQuat = XMLoadFloat4(&transform.rotation);
	const XMMATRIX targetRotMat = XMMatrixRotationQuaternion(targetRotQuat);
	const XMVECTOR forward = targetRotMat.r[2];

	// Yaw + pitch
	const float targetYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));
	const float totalYaw = targetYaw + camera.yawOffset;
	camera.targetYaw = totalYaw;
	camera.targetPitch = camera.pitchAngle;

	// Base focus
	XMVECTOR baseFocus = XMVectorAdd(targetPos, XMVectorSet(0, camera.heightOffset, 0, 0));

	// FIRST PERSON
	if (camera.mode == Camera::CameraMode::FIRST_PERSON || camera.isTemporaryFirstPerson)
	{
		XMVECTOR fpOffset = XMLoadFloat3(&camera.firstPersonOffset);
		XMVECTOR s = XMLoadFloat3(&transform.scale);
		fpOffset = XMVectorMultiply(fpOffset, s);
		camera.position = XMVectorAdd(targetPos, XMVector3Transform(fpOffset, targetRotMat));

		// Forward basé sur yaw/pitch
		const XMMATRIX yawPitchMat = XMMatrixRotationRollPitchYaw(camera.pitchAngle, camera.yawOffset + targetYaw, 0.0f);
		XMVECTOR fpForward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), yawPitchMat);

		camera.focus = XMVectorAdd(camera.position, XMVectorScale(fpForward, 100.0f));
		camera.up = XMVectorSet(0, 1, 0, 0); // Up fixe

		if (camera.mode == Camera::CameraMode::FIRST_PERSON && !camera.isTemporaryFirstPerson)
			return;
	}

	// --- THIRD PERSON SpringArm ---
	const float horizontalDist = camera.distance * cosf(camera.pitchAngle);
	const float verticalDist = camera.distance * sinf(camera.pitchAngle);

	XMVECTOR idealCameraPos = XMVectorSet(
		XMVectorGetX(baseFocus) - horizontalDist * sinf(totalYaw),
		XMVectorGetY(baseFocus) + verticalDist + camera.heightOffset,
		XMVectorGetZ(baseFocus) - horizontalDist * cosf(totalYaw),
		1.0f
	);

	XMVECTOR adjustedPos = idealCameraPos;
	if (camera.enableSpringArm)
	{
		XMVECTOR dir = XMVectorSubtract(idealCameraPos, baseFocus);
		float targetDist = XMVectorGetX(XMVector3Length(dir));
		if (targetDist > 0.0001f)
		{
			dir = XMVector3Normalize(dir);
			float safeDist = PerformSpringArmRaycast(baseFocus, idealCameraPos, targetDist);
			adjustedPos = XMVectorAdd(baseFocus, XMVectorScale(dir, safeDist));
		}
	}

	float achievedDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(adjustedPos, baseFocus)));

	// --- Temporary 1st Person ---
	constexpr float tempFPMinTime = 2.0f;
	constexpr float fpEntryFactor = 0.1f;
	constexpr float fpExitFactor = 1.05f;
	constexpr float exitDistanceTolerance = 0.9f;
	constexpr float reentryCooldown = 0.75f;

	// Met a jour le cooldown
	if (camera.tempFPExitCooldown > 0.0f)
		camera.tempFPExitCooldown = std::max(0.0f, camera.tempFPExitCooldown - dt);

	// Détecte l'entrée forcée en temporary 1st person
	bool forceTempFP = camera.tempFPExitCooldown <= 0.0f &&
		achievedDistance < camera.distance * fpEntryFactor;

	if (!camera.isTemporaryFirstPerson && forceTempFP)
	{
		camera.isTemporaryFirstPerson = true;
		camera.distanceBeforeTemporaryFP = camera.distance;
		camera.tempFPTime = 0.0f;
		camera.justEnteredTemporaryFP = true;
	}

	if (camera.isTemporaryFirstPerson)
	{
		camera.tempFPTime += dt;

		// Position FP
		XMVECTOR fpOffset = XMLoadFloat3(&camera.firstPersonOffset);
		XMVECTOR s = XMLoadFloat3(&transform.scale);
		fpOffset = XMVectorMultiply(fpOffset, s);
		XMVECTOR rotatedOffset = XMVector3Transform(fpOffset, targetRotMat);
		camera.position = XMVectorAdd(targetPos, rotatedOffset);

		const float baseYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));
		const XMMATRIX yawPitchMat = XMMatrixRotationRollPitchYaw(camera.pitchAngle, camera.yawOffset + baseYaw, 0.0f);
		XMVECTOR fpForward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), yawPitchMat);

		camera.focus = XMVectorAdd(camera.position, XMVectorScale(fpForward, 100.0f));

		// Condition de sortie de 1rst person temporaire
		bool hasStayedMinTime = camera.tempFPTime >= tempFPMinTime;
		bool distanceRecovered = achievedDistance >= camera.distanceBeforeTemporaryFP * exitDistanceTolerance ||
			achievedDistance >= camera.distance * fpExitFactor;

		if (hasStayedMinTime && distanceRecovered)
		{
			// Reset les valeurs temporaires
			camera.isTemporaryFirstPerson = false;
			camera.tempFPExitCooldown = reentryCooldown;
			camera.tempFPTime = 0.0f;
			camera.justEnteredTemporaryFP = false;

			// Restaurer la distance stockée
			camera.distance = camera.distanceBeforeTemporaryFP;

			// Placer la caméra progressivement sur adjustedPos
			camera.position = adjustedPos;
		}
		else
		{
			if (camera.justEnteredTemporaryFP)
				camera.justEnteredTemporaryFP = false;

			return;
		}
	}

	// --- 3rd Person interpolation (après gestion temporaryFP) ---
	float lerpSpeed = camera.springArmSpeed;
	if (achievedDistance < camera.distance * 0.5f)
		lerpSpeed *= 3.0f;

	camera.position = adjustedPos;

	// Focus latéral
	XMVECTOR idealToActual = XMVectorSubtract(camera.position, idealCameraPos);
	float lateralDisplacement = XMVectorGetX(XMVector3Length(idealToActual));
	if (lateralDisplacement > camera.distance * 0.005f)
	{
		XMVECTOR lateralDirection = XMVector3Normalize(idealToActual);
		camera.focus = XMVectorAdd(baseFocus, XMVectorScale(lateralDirection, lateralDisplacement * 0.95f));
	}
	else
		camera.focus = baseFocus;

	Camera::currentDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(camera.position, camera.focus)));
}


void CameraSystem::ComputeCameraOrientation(Camera& camera)
{
	const XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(camera.focus, camera.position));
	const XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);
	const XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, forward));

	camera.up = XMVector3Normalize(XMVector3Cross(forward, right));
}

void CameraSystem::UpdateCameraMatrices(Camera& camera, const EntityManager& entityManager, float dt)
{
	const auto targetTransform = entityManager.Get<Transform>(camera.targetEntity);

	ComputeCameraPosition(camera, targetTransform, dt);
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
		if (cameraData.mode == Camera::CameraMode::FIRST_PERSON || cameraData.isTemporaryFirstPerson) {
			yawSensitivity = 0.00002f;
			pitchSensitivity = 0.0004f;
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
			float returnSpeed;
			if (cameraData.mode == Camera::CameraMode::FIRST_PERSON)
			{
				returnSpeed = 0.005f;
			}
			else
			{
				returnSpeed = 0.02f;
			}

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

void CameraSystem::ScaleCamera(float scaleFactor)
{
	float adjustedScaleFactor = scaleFactor * 0.9f;

	// Calculer les valeurs à atteindre
	const float targetDistance = Camera::distance * adjustedScaleFactor;
	const float targetHeightOffset = Camera::heightOffset * adjustedScaleFactor;
	const float targetMinDistance = Camera::minDistance * adjustedScaleFactor;
	const float targetMaxDistance = Camera::maxDistance * adjustedScaleFactor;
	const float targetZoomSpeed = Camera::zoomSpeed * adjustedScaleFactor;

	// Calculer les deltas
	Camera::deltaDistance = targetDistance - Camera::distance;
	Camera::deltaHeightOffset = targetHeightOffset - Camera::heightOffset;
	Camera::deltaMinDistance = targetMinDistance - Camera::minDistance;
	Camera::deltaMaxDistance = targetMaxDistance - Camera::maxDistance;
	Camera::deltaZoomSpeed = targetZoomSpeed - Camera::zoomSpeed;

	// Comme pour la croissance du personnage
	const float F_GROWTH_STEPS = 60.0f;
	Camera::cameraScaleStep = Camera::deltaDistance / F_GROWTH_STEPS;
}

float CameraSystem::PerformSpringArmRaycast(const XMVECTOR& start, const XMVECTOR& end, float maxDistance)
{
	auto& physicSystem = JoltSystem::GetPhysicSystem();

	// Convertir en Jolt
	JPH::RVec3 rayStart(XMVectorGetX(start), XMVectorGetY(start), XMVectorGetZ(start));
	JPH::RVec3 rayEnd(XMVectorGetX(end), XMVectorGetY(end), XMVectorGetZ(end));

	JPH::Vec3 rayDirection = (rayEnd - rayStart).Normalized();
	const float rayLength = (rayEnd - rayStart).Length();

	if (rayLength < 0.001f)
		return maxDistance;

	// Créer un cast "épais" pour ne pas voir les "intérieurs" de côté
	JPH::RefConst<JPH::SphereShape> sphere = new JPH::SphereShape(Camera::cameraRadius);

	JPH::RShapeCast shapeCast = JPH::RShapeCast::sFromWorldTransform(
		sphere,
		JPH::Vec3::sReplicate(1.0f),  // Scale
		JPH::RMat44::sTranslation(rayStart),  // Position de départ
		rayDirection * rayLength  // Direction et longueur
	);

	// Hit le plus proche
	class ClosestHitCollector : public JPH::CastShapeCollector
	{
	public:
		virtual void AddHit(const JPH::ShapeCastResult& inResult) override
		{
			if (inResult.mFraction < mHit.mFraction)
			{
				mHit = inResult;
			}
		}

		JPH::ShapeCastResult mHit;
	};

	ClosestHitCollector collector;
	collector.mHit.mFraction = 1.0f;

	// Collide sur NON_MOVING
	class NonMovingOnlyBroadPhaseFilter : public JPH::BroadPhaseLayerFilter
	{
	public:
		virtual bool ShouldCollide(JPH::BroadPhaseLayer inLayer) const override
		{
			return inLayer == BroadPhaseLayers::NON_MOVING;
		}
	};

	class NonMovingOnlyObjectFilter : public JPH::ObjectLayerFilter
	{
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer inLayer) const override
		{
			return inLayer == Layers::NON_MOVING;
		}
	};

	class AllBodyFilter : public JPH::BodyFilter
	{
	public:
		virtual bool ShouldCollide([[maybe_unused]] const JPH::BodyID& inBodyID) const override
		{
			return true;
		}
	};

	class NoShapeFilter : public JPH::ShapeFilter
	{
	public:
		virtual bool ShouldCollide([[maybe_unused]] const JPH::Shape* inShape2, [[maybe_unused]] const JPH::SubShapeID& inSubShapeIDOfShape2) const override
		{
			return true;
		}
	};

	NonMovingOnlyBroadPhaseFilter broadPhaseFilter;
	NonMovingOnlyObjectFilter objectLayerFilter;
	AllBodyFilter bodyFilter;
	NoShapeFilter shapeFilter;

	JPH::ShapeCastSettings settings;
	settings.mUseShrunkenShapeAndConvexRadius = true;
	settings.mReturnDeepestPoint = false;

	// Effectuer le cast
	physicSystem.GetNarrowPhaseQuery().CastShape(
		shapeCast,
		settings,
		JPH::RVec3::sZero(),
		collector,
		broadPhaseFilter,
		objectLayerFilter,
		bodyFilter,
		shapeFilter
	);

	if (collector.mHit.mFraction < 1.0f)
	{
		const float hitDistance = collector.mHit.mFraction * rayLength;

		// Offset pour tenir compte de la sphère
		const float COLLISION_OFFSET = Camera::cameraRadius + Camera::collisionOffset;
		return std::max(hitDistance - COLLISION_OFFSET, 0.1f);
	}

	return maxDistance;
}

// Pour l'interpolation
float CameraSystem::Lerp(float a, float b, float t)
{
	return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
}