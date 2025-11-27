#include "pch.h"
#include "rendering/graphics/Billboard.h"

Billboard::Billboard(const ShaderProgram& shaderProgram_, const Texture& texture_, ID3D11Device* device,
                     const DirectX::XMFLOAT3 position, const DirectX::XMFLOAT2 scale)
	: position{position},
	  scale{scale},
	  texture{texture_},
	  shaderProgram{shaderProgram_},
	  vertexBuffer{device, ComputeVertices()}
{
}

std::vector<VertexSprite> Billboard::ComputeVertices()
{
    return
    {
        {{-0.5f, -0.5f, 0.0f}, {0, 1}}, // bottom-left
        {{ 0.5f, -0.5f, 0.0f}, {1, 1}}, // bottom-right
        {{-0.5f,  0.5f, 0.0f}, {0, 0}}, // top-left

        {{ 0.5f, -0.5f, 0.0f}, {1, 1}}, // bottom-right
        {{ 0.5f,  0.5f, 0.0f}, {1, 0}}, // top-right
        {{-0.5f,  0.5f, 0.0f}, {0, 0}}  // top-left
    };
}
