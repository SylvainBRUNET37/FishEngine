#ifndef MATERIAL_H
#define MATERIAL_H

#include "rendering/utils/ComPtr.h"

struct Material
{
	Material()
	{
		ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		transparent = false;
		shininess = 0;
		textureFileName = "";
	}

	void Bind(ID3D11DeviceContext* context)
	{
		context->PSSetShaderResources(0, 1, &texture);
	}

	std::string textureFileName;
	std::string materialName;

	ComPtr<ID3D11ShaderResourceView> texture;

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	float shininess;
	bool transparent;
};

#endif
