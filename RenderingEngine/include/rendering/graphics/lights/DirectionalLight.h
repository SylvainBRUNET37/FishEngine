#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include <DirectXMath.h>

struct DirectionalLight
{
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 specular;

    DirectX::XMFLOAT3 direction;
    float pad; // alignment
};

#endif