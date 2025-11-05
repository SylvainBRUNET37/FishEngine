#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <DirectXMath.h>

#include "rendering/graphics/lights/DirectionalLight.h"
#include "rendering/graphics/lights/PointLight.h"

struct alignas(16) FrameBuffer // b0 in the shader program
{
	static constexpr size_t MAX_POINT_LIGHTS = 1;

	DirectX::XMMATRIX matViewProj;
	DirectX::XMFLOAT4 vCamera;
    DirectionalLight dirLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
};

#endif