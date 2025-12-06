#ifndef SHADOWMAPLIGHTWVP_BUFFER_H
#define SHADOWMAPLIGHTWVP_BUFFER_H

#include <DirectXMath.h>

struct alignas(16) ShadowMapLightWVPBuffer // b0 in the shader program
{
	DirectX::XMMATRIX lightWVP;
};

#endif