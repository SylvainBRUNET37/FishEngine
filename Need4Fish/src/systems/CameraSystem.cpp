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

	// NOUVEAU: Vérifier si on doit être en mode temporaire première personne
	bool wasTemporaryFP = Camera::isTemporaryFirstPerson;

	if (camera.mode == Camera::CameraMode::FIRST_PERSON || Camera::isTemporaryFirstPerson)
	{
		XMVECTOR fpOffset = XMLoadFloat3(&camera.firstPersonOffset);
		XMVECTOR s = XMLoadFloat3(&transform.scale);
		fpOffset = XMVectorMultiply(fpOffset, s);

		const XMVECTOR rotatedOffset = XMVector3Transform(fpOffset, targetRotMat);
		camera.position = XMVectorAdd(targetPos, rotatedOffset);

		const XMVECTOR lookDirection = XMVector3Normalize(forward);
		camera.focus = XMVectorAdd(camera.position, XMVectorScale(lookDirection, 100.0f));

		// NOUVEAU: Si on était en mode temporaire, vérifier si on peut revenir en 3e personne
		if (Camera::isTemporaryFirstPerson && camera.mode == Camera::CameraMode::THIRD_PERSON)
		{
			// Essayer de revenir à la distance d'avant
			const float testDistance = Camera::distanceBeforeTemporaryFP;
			const float horizontalDist = testDistance * cosf(camera.pitchAngle);
			const float verticalDist = testDistance * sinf(camera.pitchAngle);

			XMVECTOR baseFocus = XMVectorAdd(targetPos, XMVectorSet(0, camera.heightOffset, 0, 0));
			const XMVECTOR testCameraPos = XMVectorSet(
				XMVectorGetX(baseFocus) - horizontalDist * sinf(totalYaw),
				XMVectorGetY(baseFocus) + verticalDist + camera.heightOffset,
				XMVectorGetZ(baseFocus) - horizontalDist * cosf(totalYaw),
				1.0f
			);

			// Vérifier s'il y a encore une collision à cette distance
			XMVECTOR adjustedTestPos = PerformSpringArm3D(baseFocus, testCameraPos, testDistance);
			float achievedDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(adjustedTestPos, baseFocus)));

			// Si on peut atteindre au moins 80% de la distance d'origine, revenir en 3e personne
			if (achievedDistance >= testDistance * 0.8f)
			{
				Camera::isTemporaryFirstPerson = false;
				// Continuer le traitement en 3e personne ci-dessous
			}
			else
			{
				// Rester en première personne temporaire
				return;
			}
		}
		else
		{
			return;
		}
	}

	// THIRD_PERSON (ou sortie du mode temporaire)
	if (camera.mode == Camera::CameraMode::THIRD_PERSON)
	{
		XMVECTOR baseFocus = XMVectorAdd(targetPos, XMVectorSet(0, camera.heightOffset, 0, 0));

		const float horizontalDist = camera.distance * cosf(camera.pitchAngle);
		const float verticalDist = camera.distance * sinf(camera.pitchAngle);

		const XMVECTOR idealCameraPos = XMVectorSet(
			XMVectorGetX(baseFocus) - horizontalDist * sinf(totalYaw),
			XMVectorGetY(baseFocus) + verticalDist + camera.heightOffset,
			XMVectorGetZ(baseFocus) - horizontalDist * cosf(totalYaw),
			1.0f
		);

		if (camera.enableSpringArm)
		{
			const float desiredDistance = camera.distance;

			if (Camera::currentDistance <= 0.0f)
				Camera::currentDistance = desiredDistance;

			XMVECTOR adjustedPos = PerformSpringArm3D(baseFocus, idealCameraPos, desiredDistance);

			const float t = std::clamp(camera.springArmSpeed * dt, 0.0f, 1.0f);
			camera.position = XMVectorLerp(camera.position, adjustedPos, t);

			// NOUVEAU: Calculer la distance réelle atteinte
			float achievedDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(camera.position, baseFocus)));

			// NOUVEAU: Si la caméra est trop proche (moins de 15% de la distance désirée),
			// passer temporairement en première personne
			const float fpThreshold = desiredDistance * 0.15f; // 15% de la distance

			if (achievedDistance < fpThreshold && !Camera::isTemporaryFirstPerson)
			{
				Camera::isTemporaryFirstPerson = true;
				Camera::distanceBeforeTemporaryFP = desiredDistance;

				// Recalculer immédiatement en première personne
				XMVECTOR fpOffset = XMLoadFloat3(&camera.firstPersonOffset);
				XMVECTOR s = XMLoadFloat3(&transform.scale);
				fpOffset = XMVectorMultiply(fpOffset, s);

				const XMVECTOR rotatedOffset = XMVector3Transform(fpOffset, targetRotMat);
				camera.position = XMVectorAdd(targetPos, rotatedOffset);

				const XMVECTOR lookDirection = XMVector3Normalize(forward);
				camera.focus = XMVectorAdd(camera.position, XMVectorScale(lookDirection, 100.0f));

				return;
			}

			XMVECTOR idealToActual = XMVectorSubtract(camera.position, idealCameraPos);
			float lateralDisplacement = XMVectorGetX(XMVector3Length(idealToActual));

			float distanceThreshold = desiredDistance * 0.005f;

			if (lateralDisplacement > distanceThreshold)
			{
				XMVECTOR lateralDirection = XMVector3Normalize(idealToActual);
				float focusDisplacement = lateralDisplacement * 0.95f;
				camera.focus = XMVectorAdd(baseFocus, XMVectorScale(lateralDirection, focusDisplacement));
			}
			else
			{
				camera.focus = baseFocus;
			}

			Camera::currentDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(camera.position, camera.focus)));
		}
		else
		{
			camera.position = idealCameraPos;
			camera.focus = baseFocus;
		}
	}
}

XMVECTOR CameraSystem::PerformSpringArm3D(const XMVECTOR& focus, const XMVECTOR& idealPos, float maxDistance)
{
	auto& physicSystem = JoltSystem::GetPhysicSystem();

	JPH::RVec3 focusPos(XMVectorGetX(focus), XMVectorGetY(focus), XMVectorGetZ(focus));
	JPH::RVec3 idealCamPos(XMVectorGetX(idealPos), XMVectorGetY(idealPos), XMVectorGetZ(idealPos));

	JPH::Vec3 direction = (idealCamPos - focusPos).Normalized();
	const float totalDistance = (idealCamPos - focusPos).Length();

	if (totalDistance < 0.001f)
		return idealPos;

	JPH::RefConst<JPH::SphereShape> sphere = new JPH::SphereShape(Camera::cameraRadius);

	JPH::RShapeCast shapeCast = JPH::RShapeCast::sFromWorldTransform(
		sphere,
		JPH::Vec3::sReplicate(1.0f),
		JPH::RMat44::sTranslation(focusPos),
		direction * totalDistance
	);

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

		float safePosDistance = max(hitDistance - safetyOffset, Camera::cameraRadius * 2.0f);
		safePosDistance = min(safePosDistance, maxDistance);

		JPH::RVec3 safePos = focusPos + direction * safePosDistance;

		JPH::Vec3 contactNormal = -collector.mHit.mPenetrationAxis.Normalized();
		float dotProduct = direction.Dot(contactNormal);

		const float normalY = contactNormal.GetY();
		const float verticalThreshold = 0.6f;

		const bool isVerticalCollision = std::abs(normalY) > verticalThreshold;
		bool verticalAdjustmentApplied = false; // NOUVEAU: Flag pour éviter la validation finale

		if (isVerticalCollision)
		{
			// === GESTION DES COLLISIONS VERTICALES ===

			JPH::Vec3 verticalUp = JPH::Vec3(0.0f, 1.0f, 0.0f);

			float collisionSeverity = 1.0f - collector.mHit.mFraction;

			// CHANGEMENT: Offset beaucoup plus agressif pour les plafonds
			float maxVerticalOffset = safetyOffset * 10.0f; // Augmenté de 6.0f à 10.0f
			float verticalOffset = maxVerticalOffset * collisionSeverity;

			// CHANGEMENT: Pas de limite maximale pour le déplacement vertical
			// On veut pouvoir descendre autant que nécessaire

			if (normalY < 0.0f) // Plafond (normale vers le bas)
			{
				// DESCENDRE DE MANIÈRE TRÈS AGRESSIVE

				// Essayer de descendre beaucoup plus bas
				JPH::RVec3 verticalPosDown = safePos - JPH::RVec3(verticalUp) * verticalOffset;

				// Validation
				JPH::RShapeCast verticalValidationCastDown = JPH::RShapeCast::sFromWorldTransform(
					sphere,
					JPH::Vec3::sReplicate(1.0f),
					JPH::RMat44::sTranslation(focusPos),
					(verticalPosDown - focusPos)
				);

				ClosestHitCollector verticalCollectorDown;
				verticalCollectorDown.mHit.mFraction = 1.0f;

				physicSystem.GetNarrowPhaseQuery().CastShape(
					verticalValidationCastDown,
					settings,
					JPH::RVec3::sZero(),
					verticalCollectorDown,
					broadPhaseFilter,
					objectLayerFilter,
					bodyFilter,
					shapeFilter
				);

				// CHANGEMENT: Validation beaucoup plus permissive (0.3f au lieu de 0.5f)
				if (verticalCollectorDown.mHit.mFraction >= 0.3f)
				{
					if (verticalCollectorDown.mHit.mFraction < 1.0f)
					{
						float adjustedOffset = verticalOffset * verticalCollectorDown.mHit.mFraction * 0.95f;
						safePos = safePos - JPH::RVec3(verticalUp) * adjustedOffset;
					}
					else
					{
						safePos = verticalPosDown;
					}
					verticalAdjustmentApplied = true; // On a réussi à descendre
				}
				else
				{
					// Si on ne peut pas descendre autant, descendre au maximum possible
					// CHANGEMENT: Descendre quand même un peu, même avec collision
					float adjustedOffset = verticalOffset * verticalCollectorDown.mHit.mFraction * 0.95f;
					if (adjustedOffset > safetyOffset) // Au moins descendre d'un peu
					{
						safePos = safePos - JPH::RVec3(verticalUp) * adjustedOffset;
						verticalAdjustmentApplied = true;
					}
				}
			}
			else // Sol (normale vers le haut)
			{
				// Essayer de monter
				JPH::RVec3 verticalPosUp = safePos + JPH::RVec3(verticalUp) * verticalOffset;

				JPH::RShapeCast verticalValidationCastUp = JPH::RShapeCast::sFromWorldTransform(
					sphere,
					JPH::Vec3::sReplicate(1.0f),
					JPH::RMat44::sTranslation(focusPos),
					(verticalPosUp - focusPos)
				);

				ClosestHitCollector verticalCollectorUp;
				verticalCollectorUp.mHit.mFraction = 1.0f;

				physicSystem.GetNarrowPhaseQuery().CastShape(
					verticalValidationCastUp,
					settings,
					JPH::RVec3::sZero(),
					verticalCollectorUp,
					broadPhaseFilter,
					objectLayerFilter,
					bodyFilter,
					shapeFilter
				);

				if (verticalCollectorUp.mHit.mFraction >= 0.3f)
				{
					if (verticalCollectorUp.mHit.mFraction < 1.0f)
					{
						float adjustedOffset = verticalOffset * verticalCollectorUp.mHit.mFraction * 0.95f;
						safePos = safePos + JPH::RVec3(verticalUp) * adjustedOffset;
					}
					else
					{
						safePos = verticalPosUp;
					}
					verticalAdjustmentApplied = true;
				}
			}
		}
		else
		{
			// === GESTION DES COLLISIONS LATÉRALES ===

			const float perpendicularThreshold = 0.6f;

			if (std::abs(dotProduct) < perpendicularThreshold)
			{
				JPH::Vec3 lateralComponent = contactNormal - direction * dotProduct;

				if (lateralComponent.Length() > 0.001f)
				{
					lateralComponent = lateralComponent.Normalized();

					float collisionSeverity = 1.0f - collector.mHit.mFraction;
					float maxLateralOffset = safetyOffset * 6.0f;
					float lateralOffset = maxLateralOffset * collisionSeverity;

					JPH::RVec3 lateralPos1 = safePos + JPH::RVec3(lateralComponent) * lateralOffset;
					JPH::RVec3 lateralPos2 = safePos - JPH::RVec3(lateralComponent) * lateralOffset;

					JPH::RShapeCast lateralValidationCast1 = JPH::RShapeCast::sFromWorldTransform(
						sphere,
						JPH::Vec3::sReplicate(1.0f),
						JPH::RMat44::sTranslation(focusPos),
						(lateralPos1 - focusPos)
					);

					ClosestHitCollector lateralCollector1;
					lateralCollector1.mHit.mFraction = 1.0f;

					physicSystem.GetNarrowPhaseQuery().CastShape(
						lateralValidationCast1,
						settings,
						JPH::RVec3::sZero(),
						lateralCollector1,
						broadPhaseFilter,
						objectLayerFilter,
						bodyFilter,
						shapeFilter
					);

					JPH::RShapeCast lateralValidationCast2 = JPH::RShapeCast::sFromWorldTransform(
						sphere,
						JPH::Vec3::sReplicate(1.0f),
						JPH::RMat44::sTranslation(focusPos),
						(lateralPos2 - focusPos)
					);

					ClosestHitCollector lateralCollector2;
					lateralCollector2.mHit.mFraction = 1.0f;

					physicSystem.GetNarrowPhaseQuery().CastShape(
						lateralValidationCast2,
						settings,
						JPH::RVec3::sZero(),
						lateralCollector2,
						broadPhaseFilter,
						objectLayerFilter,
						bodyFilter,
						shapeFilter
					);

					if (lateralCollector1.mHit.mFraction >= 0.5f || lateralCollector2.mHit.mFraction >= 0.5f)
					{
						if (lateralCollector1.mHit.mFraction > lateralCollector2.mHit.mFraction)
						{
							if (lateralCollector1.mHit.mFraction < 1.0f)
							{
								float adjustedOffset = lateralOffset * lateralCollector1.mHit.mFraction * 0.9f;
								safePos = safePos + JPH::RVec3(lateralComponent) * adjustedOffset;
							}
							else
							{
								safePos = lateralPos1;
							}
						}
						else
						{
							if (lateralCollector2.mHit.mFraction < 1.0f)
							{
								float adjustedOffset = lateralOffset * lateralCollector2.mHit.mFraction * 0.9f;
								safePos = safePos - JPH::RVec3(lateralComponent) * adjustedOffset;
							}
							else
							{
								safePos = lateralPos2;
							}
						}
					}
				}
			}
		}

		// CHANGEMENT CRITIQUE: Validation finale SEULEMENT si on n'a PAS fait d'ajustement vertical
		// Si on a descendu pour un plafond, on NE VÉRIFIE PAS à nouveau (sinon ça annule la descente)
		if (!verticalAdjustmentApplied)
		{
			JPH::RShapeCast validationCast = JPH::RShapeCast::sFromWorldTransform(
				sphere,
				JPH::Vec3::sReplicate(1.0f),
				JPH::RMat44::sTranslation(focusPos),
				(safePos - focusPos)
			);

			ClosestHitCollector validationCollector;
			validationCollector.mHit.mFraction = 1.0f;

			physicSystem.GetNarrowPhaseQuery().CastShape(
				validationCast,
				settings,
				JPH::RVec3::sZero(),
				validationCollector,
				broadPhaseFilter,
				objectLayerFilter,
				bodyFilter,
				shapeFilter
			);

			if (validationCollector.mHit.mFraction < 0.5f)
			{
				const float validatedDistance = validationCollector.mHit.mFraction *
					(safePos - focusPos).Length();
				safePosDistance = max(validatedDistance * 0.9f - safetyOffset, Camera::cameraRadius * 2.0f);
				safePos = focusPos + direction * safePosDistance;
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

	static constexpr float deadzone = 2.0f; // Pixels de tolérance

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