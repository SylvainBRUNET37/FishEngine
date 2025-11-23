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

#include "rendering/core/Transform.h"
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

// ===============================================================
// Add new component types here to register them with the engine
// ===============================================================

using Components = std::tuple
<
	Mesh,
	RigidBody,
	Transform,
	Hierarchy,
	Name,
	Controllable,
	Camera,
	PointLight,
	Eatable,
	PowerSource
>;

#endif
