#ifndef SPRITE_H
#define SPRITE_H

#include "VertexSprite.h"
#include "rendering/buffers/VertexBuffer.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/texture/Texture.h"

struct Sprite
{
    struct SpritePosition
    {
        float x;
        float y;
    };

    explicit Sprite(const ShaderProgram& shaderProgram, const Texture& texture, ID3D11Device* device);

    Texture texture;
    SpritePosition position;

    VertexBuffer vertexBuffer;
    ShaderProgram shaderProgram;

private:
    static std::vector<VertexSprite> ComputeVertices(const SpritePosition& position, const Texture& texture);
};

#endif