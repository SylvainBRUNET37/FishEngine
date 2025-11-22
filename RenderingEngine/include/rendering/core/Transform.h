#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <DirectXMath.h>

struct Transform
{
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
    DirectX::XMFLOAT3 position{ 0, 0, 0 };
    DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 };
    DirectX::XMFLOAT3 scale{ 1, 1, 1 };
    DirectX::XMFLOAT3 deltaScale{ 0, 0, 0 };
    float scaleStep = 0.0f;
};

#endif