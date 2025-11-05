#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <DirectXMath.h>

struct PointLight
{
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 specular;

    DirectX::XMFLOAT3 position;
    float range; // not used yet but may be useful in the future
    DirectX::XMFLOAT3 attenuation;
    float pad; // alignment
};

#endif