#ifndef MATERIAL_H
#define MATERIAL_H

#include <DirectXMath.h>
#include <memory>
#include <string>

#include "rendering/buffers/constantBuffers/ConstantBuffer.h"
#include "rendering/buffers/constantBuffers/MaterialBuffer.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/utils/ComPtr.h"

struct Material
{
	explicit Material(ID3D11Device* device, const std::shared_ptr<ShaderProgram>& shaderProgram, const int materialCbRegisterNumber)
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

	ConstantBuffer<MaterialBuffer> constantBuffer; // b2 in the shader program
	std::shared_ptr<ShaderProgram> shaderProgram;
};

#endif
