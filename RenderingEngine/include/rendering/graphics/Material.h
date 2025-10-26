#ifndef MATERIAL_H
#define MATERIAL_H

#include <DirectXMath.h>
#include <string>

#include "rendering/buffers/ConstantBuffer.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/utils/ComPtr.h"

struct Material
{
	struct MaterialBufferData // b2 in the shader program
	{
		DirectX::XMFLOAT4 vAMat;
		DirectX::XMFLOAT4 vDMat;
		DirectX::XMFLOAT4 vSMat;
		float puissance;
		float bTex;
		DirectX::XMFLOAT2 padding; // align to 16 bytes
	};

	explicit Material(ID3D11Device* device, const ShaderProgram& shaderProgram, const int materialCbRegisterNumber)
		: constantBuffer{device, materialCbRegisterNumber}, shaderProgram{shaderProgram}
	{
	}

	std::string name;

	ComPtr<ID3D11ShaderResourceView> texture;

	DirectX::XMFLOAT4 ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	float shininess = 0;
	bool transparent = false;

	ConstantBuffer<MaterialBufferData> constantBuffer; // b2 in the shader program
	ShaderProgram shaderProgram;
};

#endif
