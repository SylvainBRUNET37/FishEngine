#ifndef BILLBOARD_BUFFER_H
#define BILLBOARD_BUFFER_H

#include <DirectXMath.h>

struct alignas(16) BillboardCameraBuffer
{
	DirectX::XMFLOAT4X4 matView;
	DirectX::XMFLOAT4X4 matProj;
	DirectX::XMFLOAT3 cameraPos;
};

struct alignas(16) BillboardBuffer
{
	DirectX::XMFLOAT4X4 matWorld;
};

struct alignas(16) BillboardData
{
	DirectX::XMFLOAT3 pos;
	float scale;
};

#endif