#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <DirectXMath.h>

#include "rendering/graphics/lights/DirectionalLight.h"
#include "rendering/graphics/lights/PointLight.h"

struct alignas(16) FrameBuffer // b0 in the shader program
{
	static constexpr size_t MAX_POINT_LIGHTS = 16; // should be the same in the shader program

	DirectX::XMFLOAT4X4 matViewProj;
	DirectX::XMFLOAT4 vCamera;
    DirectionalLight dirLight;

	int pointLightCount{};
	float elapsedTime{};
	DirectX::XMFLOAT2 padding_{};

    PointLight pointLights[MAX_POINT_LIGHTS];
};

#endif