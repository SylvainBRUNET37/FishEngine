#ifndef BILLBOARD_BUFFER_H
#define BILLBOARD_BUFFER_H

#include <DirectXMath.h>

struct alignas(16) BillboardBuffer
{
	DirectX::XMFLOAT4X4 matWorld;
	DirectX::XMFLOAT4X4 matView;
	DirectX::XMFLOAT4X4 matProj;
};

#endif