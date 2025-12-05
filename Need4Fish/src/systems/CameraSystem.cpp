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
	const float targetYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));
	const float totalYaw = targetYaw + camera.yawOffset;
	camera.targetYaw = totalYaw;
	camera.targetPitch = camera.pitchAngle;

	XMVECTOR baseFocus = XMVectorAdd(targetPos, XMVectorSet(0, camera.heightOffset, 0, 0));

	// Facteurs pour passage 1ère personne
	const float fpEntryFactor = 0.75f; // Plus grand -> entrée plus rapide
	const float fpExitFactor = 1.1f; // Plus grand -> sortie plus difficile.

	const float horizontalDist = camera.distance * cosf(camera.pitchAngle);
	const float verticalDist = camera.distance * sinf(camera.pitchAngle);

	// Position idéale de la caméra
	XMVECTOR idealCameraPos = XMVectorSet(
		XMVectorGetX(baseFocus) - horizontalDist * sinf(totalYaw),
		XMVectorGetY(baseFocus) + verticalDist + camera.heightOffset,
		XMVectorGetZ(baseFocus) - horizontalDist * cosf(totalYaw),
		1.0f
	);

	// Application du spring arm
	XMVECTOR adjustedPos = idealCameraPos;
	if (camera.enableSpringArm)
		adjustedPos = PerformSpringArm3D(baseFocus, idealCameraPos, camera.distance);

	float achievedDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(adjustedPos, baseFocus)));

	// Gestion temporaire du passage en première personne
	static float tempFPTime = 0.0f;
	const float tempFPMinTime = 2.f;
	bool forceFirstPerson = (achievedDistance < camera.distance * fpEntryFactor);

	if (!Camera::isTemporaryFirstPerson && forceFirstPerson)
	{
		Camera::isTemporaryFirstPerson = true;
		Camera::distanceBeforeTemporaryFP = camera.distance;
		tempFPTime = 0.0f;
	}

	if (Camera::isTemporaryFirstPerson)
	{
		tempFPTime += dt;

		XMVECTOR fpOffset = XMLoadFloat3(&camera.firstPersonOffset);
		XMVECTOR s = XMLoadFloat3(&transform.scale);
		fpOffset = XMVectorMultiply(fpOffset, s);
		XMVECTOR rotatedOffset = XMVector3Transform(fpOffset, targetRotMat);

		camera.position = XMVectorAdd(targetPos, rotatedOffset);
		camera.focus = XMVectorAdd(camera.position, XMVectorScale(forward, 100.0f));

		if (achievedDistance > camera.distance * fpExitFactor && tempFPTime > tempFPMinTime)
			Camera::isTemporaryFirstPerson = false;

		return;
	}

	// Caméra 3ème personne
	float lerpSpeed = camera.springArmSpeed;
	if (achievedDistance < camera.distance * 0.5f)
		lerpSpeed *= 3.0f;

	camera.position = XMVectorLerp(camera.position, adjustedPos, std::clamp(lerpSpeed * dt, 0.0f, 1.0f));

	// Gestion du focus latéral
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

XMVECTOR CameraSystem::PerformSpringArm3D(const XMVECTOR& focus, const XMVECTOR& idealPos, float maxDistance)
{
	auto& physicSystem = JoltSystem::GetPhysicSystem();
	JPH::RVec3 focusPos(XMVectorGetX(focus), XMVectorGetY(focus), XMVectorGetZ(focus));
	JPH::RVec3 idealCamPos(XMVectorGetX(idealPos), XMVectorGetY(idealPos), XMVectorGetZ(idealPos));

	JPH::Vec3 direction = (idealCamPos - focusPos).Normalized();
	const float totalDistance = (idealCamPos - focusPos).Length();
	if (totalDistance < 0.001f) return idealPos;

	JPH::RefConst<JPH::SphereShape> sphere = new JPH::SphereShape(Camera::cameraRadius);
	JPH::RShapeCast shapeCast = JPH::RShapeCast::sFromWorldTransform(
		sphere, JPH::Vec3::sReplicate(1.0f), JPH::RMat44::sTranslation(focusPos), direction * totalDistance);

	class ClosestHitCollector : public JPH::CastShapeCollector
	{
	public:
		void AddHit(const JPH::ShapeCastResult& inResult) override
		{
			if (inResult.mFraction < mHit.mFraction)
				mHit = inResult;
		}
		JPH::ShapeCastResult mHit;
	};

	ClosestHitCollector collector;
	collector.mHit.mFraction = 1.0f;

	// Filtres de collision
	class NonMovingOnlyBroadPhaseFilter : public JPH::BroadPhaseLayerFilter { bool ShouldCollide(JPH::BroadPhaseLayer inLayer) const override { return inLayer == BroadPhaseLayers::NON_MOVING; } };
	class NonMovingOnlyObjectFilter : public JPH::ObjectLayerFilter { bool ShouldCollide(JPH::ObjectLayer inLayer) const override { return inLayer == Layers::NON_MOVING; } };
	class AllBodyFilter : public JPH::BodyFilter { bool ShouldCollide(const JPH::BodyID&) const override { return true; } };
	class NoShapeFilter : public JPH::ShapeFilter { bool ShouldCollide(const JPH::Shape*, const JPH::SubShapeID&) const override { return true; } };

	NonMovingOnlyBroadPhaseFilter broadPhaseFilter;
	NonMovingOnlyObjectFilter objectLayerFilter;
	AllBodyFilter bodyFilter;
	NoShapeFilter shapeFilter;

	JPH::ShapeCastSettings settings;
	settings.mUseShrunkenShapeAndConvexRadius = true;

	// Exécution du cast
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
		const float hitDistance = collector.mHit.mFraction * totalDistance;
		const float safetyOffset = Camera::cameraRadius + Camera::collisionOffset;
		float safePosDistance = std::clamp(hitDistance - safetyOffset, Camera::cameraRadius * 2.0f, maxDistance);
		JPH::RVec3 safePos = focusPos + direction * safePosDistance;

		JPH::Vec3 normal = -collector.mHit.mPenetrationAxis.Normalized();

		// Gestion verticale stricte
		if (std::abs(normal.GetY()) > 0.3f)
		{
			JPH::RVec3 verticalUp(0, 1, 0);
			float verticalOffset = safetyOffset * 20.0f * (1.0f - collector.mHit.mFraction);
			if (normal.GetY() < 0.0f) safePos -= verticalUp * verticalOffset;
			else safePos += verticalUp * verticalOffset;
		}
		else
		{
			// Gestion latérale stricte
			float dot = direction.Dot(normal);
			if (std::abs(dot) < 0.7f)
			{
				JPH::Vec3 lateral = normal - direction * dot;
				if (lateral.Length() > 0.001f) lateral = lateral.Normalized();
				safePos += JPH::RVec3(lateral) * safetyOffset * 8.0f * (1.0f - collector.mHit.mFraction);
			}
		}

		return XMVectorSet(safePos.GetX(), safePos.GetY(), safePos.GetZ(), 1.0f);
	}

	return idealPos;
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

	static constexpr float deadzone = 2.0f;

	// Appliquer la rotation seulement si le mouvement dépasse la deadzone
	if (std::abs(deltaX) > deadzone || std::abs(deltaY) > deadzone)
	{
		// Sensibilité ajustée selon le mode
		float yawSensitivity, pitchSensitivity;
		if (cameraData.mode == Camera::CameraMode::FIRST_PERSON)
		{
			yawSensitivity = 0.00005f;
			pitchSensitivity = 0.0002f;
		}
		else
		{
			yawSensitivity = 0.002f;
			pitchSensitivity = 0.002f;
		}

		// Inversion
		const float yawMultiplier = cameraData.invertCamRotation ? -1.0f : 1.0f;
		const float pitchMultiplier = cameraData.invertCamRotation ? 1.0f : -1.0f;

		Rotate(cameraData,
			deltaX * yawSensitivity * yawMultiplier,
			deltaY * pitchSensitivity * pitchMultiplier);
	}
	else
	{
		// Ramener progressivement vers zéro
		if (std::abs(cameraData.yawOffset) > 0.001f)
		{
			float returnSpeed;
			if (cameraData.mode == Camera::CameraMode::FIRST_PERSON)
				returnSpeed = 0.005f;
			else
				returnSpeed = 0.02f;

			const float returnStep = std::copysign(returnSpeed, -cameraData.yawOffset);
			cameraData.yawOffset += returnStep;

			if (std::abs(cameraData.yawOffset) < returnSpeed)
				cameraData.yawOffset = 0.0f;
		}
	}

	// Recentrement du curseur si nécessaire
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
		cameraData.cursorCoordinates = currentCursorCoordinates;
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
	// Calculer les valeurs à atteindre
	const float targetDistance = Camera::distance * scaleFactor;
	const float targetHeightOffset = Camera::heightOffset * scaleFactor;
	const float targetMinDistance = Camera::minDistance * scaleFactor;
	const float targetMaxDistance = Camera::maxDistance * scaleFactor;
	const float targetZoomSpeed = Camera::zoomSpeed * scaleFactor;

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
		return max(hitDistance - COLLISION_OFFSET, 0.1f);
	}

	return maxDistance;
}

// Pour l'interpolation
float CameraSystem::Lerp(float a, float b, float t)
{
	return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
}