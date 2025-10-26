#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include <DirectXMath.h>

struct SceneData
{
	DirectX::XMMATRIX matViewProj;
	DirectX::XMFLOAT4 lightPosition;
    DirectX::XMFLOAT4 cameraPosition;
    DirectX::XMFLOAT4 vAEcl;
    DirectX::XMFLOAT4 vDEcl;
    DirectX::XMFLOAT4 vSEcl;
};

#endif