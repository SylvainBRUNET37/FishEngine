#ifndef POST_PROCESS_SETTINGS_H
#define POST_PROCESS_SETTINGS_H

#include <DirectXMath.h>

struct alignas(16) PostProcessSettings
{
    float closestEnnemiDistance;
    int enableChromaticAberration;
    float deltaTime;
    float padding0;

    // Unused :/
    DirectX::XMFLOAT4X4 invProjection;
    DirectX::XMFLOAT4X4 invView;
    DirectX::XMFLOAT4X4 viewProj;

    DirectX::XMFLOAT3 cameraPos;
    float padding1;
    DirectX::XMFLOAT3 sceneColorTint;

    float padding2;
};

#endif
