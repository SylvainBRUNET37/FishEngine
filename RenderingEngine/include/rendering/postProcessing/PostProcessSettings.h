#ifndef POST_PROCESS_SETTINGS_H
#define POST_PROCESS_SETTINGS_H

#include <DirectXMath.h>

struct alignas(16) PostProcessSettings
{
    int enableVignette{};
    int enableChromaticAberration{};
    float deltaTime{};
    DirectX::XMFLOAT4X4 invProjection{};
    DirectX::XMFLOAT4X4 invView{};
    DirectX::XMFLOAT4X4 viewProj{};
};

#endif
