#ifndef MATERIAL_H
#define MATERIAL_H

#include "rendering/utils/ComPtr.h"

struct Material
{
	std::string name;

	ComPtr<ID3D11ShaderResourceView> texture;

	DirectX::XMFLOAT4 ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	float shininess = 0;
	bool transparent = false;
};

#endif
