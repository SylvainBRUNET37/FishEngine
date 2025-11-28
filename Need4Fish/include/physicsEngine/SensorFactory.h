#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

struct Transform;

namespace SensorFactory
{
	JPH::Body* CreateCubeCurrentSensor(const Transform& transform);
}

#endif