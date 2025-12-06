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
	Billboard billboard;
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
	Particle
>;

#endif
