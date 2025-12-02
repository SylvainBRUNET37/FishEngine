#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "VertexSprite.h"
#include "rendering/buffers/VertexBuffer.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/texture/Texture.h"
#include <DirectXCollision.h>

struct Billboard
{
	enum Type
	{
		CameraFacing,
		Cylindric
	};

	explicit Billboard(const ShaderProgram& shaderProgram_, const Texture& texture_, ID3D11Device* device,
	                   DirectX::XMFLOAT3 position, DirectX::XMFLOAT2 scale, Type type = CameraFacing);

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 scale;

	std::vector<VertexSprite> vertices;
	Texture texture;
	ShaderProgram shaderProgram;
	VertexBuffer vertexBuffer;

	Type type;

	DirectX::BoundingBox boundingBox;

	DirectX::XMMATRIX ComputeBillboardWorldMatrix();

private:
	static std::vector<VertexSprite> ComputeVertices();

};

#endif
