#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "System.h"

class CameraSystem : public System
{
	void Update(double deltaTime, EntityManager& entityManager) override;
};

#endif