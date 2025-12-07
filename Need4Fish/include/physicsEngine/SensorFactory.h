#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H
#include "rendering/graphics/Mesh.h"

struct Entity;
struct Transform;

namespace SensorFactory
{
	JPH::Body* CreateCubeCurrentSensor(const Transform& transform, const Mesh& mesh, const Entity& entity);
	JPH::Body* CreateCubeCurrentSensor(const Transform& transform, const Entity& entity);
	JPH::Body* CreateCylinderSensor(const Transform& transform, const Entity& entity);
}

#endif