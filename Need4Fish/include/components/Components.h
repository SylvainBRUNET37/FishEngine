#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string>
#include <tuple>
#include <vector>

#include "PowerSource.h"
#include "camera/Camera.h"
#include "entities/Entity.h"
#include "gameplay/Eatable.h"
#include "physics/RigidBody.h"
#include "physics/Sensor.h"

#include "rendering/core/Transform.h"
#include "rendering/graphics/Billboard.h"
#include "rendering/graphics/lights/PointLight.h"
#include "rendering/graphics/Mesh.h"

struct Name
{
	std::string name;
};

struct Hierarchy
{
	Entity parent = INVALID_ENTITY;
	std::vector<Entity> children{};
};

struct Controllable
{
	float maxSpeed;
};

struct AIController
{
	float maxSpeed = 50.0f; // TODO: tmp
	float acceleration = 10.0f;
	float safeDistance = 200.0f;
};

struct MeshInstance
{
	UINT meshIndex;
};

struct DistortionMeshInstance
{
	UINT meshIndex;
};

struct Particle
{
	double lifeTime{}; // Store in ms the age of the object
	double lifeDuration{};
};

// ===============================================================
// Add new component types here to register them with the engine
// ===============================================================

using Components = std::tuple
<
	MeshInstance,
	RigidBody,
	Transform,
	Hierarchy,
	Name,
	Controllable,
	Camera,
	PointLight,
	Eatable,
	PowerSource,
	Billboard,
	Sensor,
	DistortionMeshInstance,
	Particle,
	AIController
>;

#endif
