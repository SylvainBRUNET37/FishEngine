#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

struct Entity;
struct Transform;

namespace SensorFactory
{
	JPH::Body* CreateCubeCurrentSensor(const Transform& transform, const Entity& entity);
}

#endif