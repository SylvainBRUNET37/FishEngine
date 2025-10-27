#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <tuple>
#include <vector>
#include <string>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "ecs/Entity.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"

// TODO: Temporary component for Devoir3, delete it after
struct Name
{
	std::string name;
};

struct Hierarchy
{
	Entity parent = INVALID_ENTITY;
	std::vector<Entity> children{};
};

struct RigidBody
{
	JPH::Body* body;
};

// The list of components ! Add a component here to add it to the engine
using Components = std::tuple
<
	Mesh,
	RigidBody,
	Transform,
	Hierarchy,
	Name
>;

#endif
