#include "pch.h"
#include "rendering/graphics/Sprite.h"

using namespace std;
using namespace DirectX;

Sprite::Sprite(const ShaderProgram& shaderProgram_, const Texture& texture_, ID3D11Device* device)
	: texture{texture_},
	  position{},
	  vertexBuffer{device, ComputeVertices(position, texture)},
	  shaderProgram{shaderProgram_}
{
}

std::vector<VertexSprite> Sprite::ComputeVertices(const SpritePosition& position, const Texture& texture)
{
	return
	{
		VertexSprite({position.x, position.y, 0}, {0, 0}), // top-left
		VertexSprite({position.x, position.y + static_cast<float>(texture.height), 0}, {0, 1}), // bottom-left
		VertexSprite({position.x + static_cast<float>(texture.width), position.y + static_cast<float>(texture.height), 0}, {1, 1}), // bottom-right
		VertexSprite({position.x, position.y, 0}, {0, 0}), // top-left
		VertexSprite({position.x + static_cast<float>(texture.width), position.y + static_cast<float>(texture.height), 0}, {1, 1}), // bottom-right
		VertexSprite({position.x + static_cast<float>(texture.width), position.y, 0}, {1, 0}) // top-right
	};
}
