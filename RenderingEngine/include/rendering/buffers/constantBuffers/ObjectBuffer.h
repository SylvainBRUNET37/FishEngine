#ifndef OBJECT_BUFFER_H
#define OBJECT_BUFFER_H

#include <DirectXMath.h>

struct ObjectBuffer // b1 in the shader program
{
	DirectX::XMMATRIX matWorld;
	DirectX::XMMATRIX shadowTransform;
};

#endif