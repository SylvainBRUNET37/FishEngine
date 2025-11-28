#ifndef SPRITE_2D_H
#define SPRITE_2D_H

#include "rendering/graphics/VertexSprite.h"
#include "rendering/buffers/VertexBuffer.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/texture/Texture.h"

struct Sprite2D
{
    struct SpritePosition
    {
        float x;
        float y;
        float z;
    };

    explicit Sprite2D(const ShaderProgram& shaderProgram, const Texture& texture, ID3D11Device* device);

    explicit Sprite2D(const ShaderProgram& shaderProgram, const Texture& texture, SpritePosition position, ID3D11Device* device);

    Texture texture;
    SpritePosition position;

    VertexBuffer vertexBuffer;
    ShaderProgram shaderProgram;

    static std::vector<VertexSprite> ComputeVertices(const SpritePosition& position, const Texture& texture);
};

#endif