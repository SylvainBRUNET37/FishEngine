#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
	Vertex() = default;

	Vertex(const DirectX::XMFLOAT3 position, const DirectX::XMFLOAT3 normal,
	       const DirectX::XMFLOAT2 textureCoord)
		: position(position), normal(normal), textureCoord(textureCoord)
	{
	}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 textureCoord;
};

#endif
