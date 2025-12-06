#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "VertexSprite.h"
#include "rendering/buffers/VertexBuffer.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/texture/Texture.h"
#include <DirectXCollision.h>
#include <memory>

struct Billboard
{
	enum Type
	{
		CameraFacing,
		Cylindric
	};

	explicit Billboard(const std::shared_ptr<ShaderProgram>& shaderProgram_, const Texture& texture_, ID3D11Device* device,
	                   DirectX::XMFLOAT3 position, DirectX::XMFLOAT2 scale, Type type = CameraFacing);

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 scale;

	std::vector<VertexSprite> vertices;
	Texture texture;
	std::shared_ptr<ShaderProgram> shaderProgram;
	VertexBuffer vertexBuffer;

	Type type;

	DirectX::BoundingBox boundingBox;

	[[nodiscard]] DirectX::XMMATRIX ComputeCylindricBillboardWorldMatrix() const;
	[[nodiscard]] DirectX::XMMATRIX ComputeCameraFacingBillboardWorldMatrix() const;

private:
	[[nodiscard]] static std::vector<VertexSprite> ComputeVertices();
	[[nodiscard]] DirectX::XMMATRIX ComputeBillboardWorldMatrix(const DirectX::XMVECTOR& directionToCamera) const;

};

#endif
