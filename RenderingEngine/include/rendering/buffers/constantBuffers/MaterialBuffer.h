#ifndef MATERIAL_BUFFER_H
#define MATERIAL_BUFFER_H

#include <DirectXMath.h>

struct MaterialBuffer // b2 in the shader program
{
	DirectX::XMFLOAT4 vAMat;
	DirectX::XMFLOAT4 vDMat;
	DirectX::XMFLOAT4 vSMat;
	float puissance;
	float bTex;
	DirectX::XMFLOAT2 padding; // align to 16 bytes
};

#endif