#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "VertexSprite.h"
#include "rendering/buffers/VertexBuffer.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/texture/Texture.h"

struct Billboard
{
	explicit Billboard(const ShaderProgram& shaderProgram_, const Texture& texture_, ID3D11Device* device,
	                   DirectX::XMFLOAT3 position, DirectX::XMFLOAT2 scale, bool isCylindric = false);

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 scale;

	Texture texture;
	ShaderProgram shaderProgram;
	VertexBuffer vertexBuffer;

	bool isCylindric;

private:
	static std::vector<VertexSprite> ComputeVertices();
};

#endif
