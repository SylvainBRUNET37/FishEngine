#ifndef RENDERER_H
#define RENDERER_H

#include "buffers/constantBuffers/FrameBuffer.h"
#include "buffers/constantBuffers/ObjectBuffer.h"
#include "buffers/constantBuffers/SpriteBuffer.h"
#include "core/Transform.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/Sprite2D.h"

class Renderer
{
public:
	explicit Renderer(ID3D11Device* device, std::vector<Material>&& materials)
		: materials{std::move(materials)},
		  frameConstantBuffer{device, frameCbRegisterNumber},
		  objectConstantBuffer{device, objectCbRegisterNumber},
		  spriteConstantBuffer{device, spriteCbRegisterNumber}
	{
	}

	void UpdateFrameBuffer(const FrameBuffer& frameBuffer_) { frameBuffer = frameBuffer_; };
	void Render(const Mesh& mesh, ID3D11DeviceContext* context, const Transform& transform);
	void Render(Sprite2D& sprite, ID3D11DeviceContext* context);
	void RenderPostProcess();

private:
	static constexpr int frameCbRegisterNumber = 0;
	static constexpr int objectCbRegisterNumber = 1;
	static constexpr int spriteCbRegisterNumber = 0;

	std::vector<Material> materials;
	FrameBuffer frameBuffer{};

	ConstantBuffer<FrameBuffer> frameConstantBuffer;
	ConstantBuffer<ObjectBuffer> objectConstantBuffer;
	ConstantBuffer<SpriteBuffer> spriteConstantBuffer;

	static void Draw(const Mesh& mesh, ID3D11DeviceContext* context);
	static void Draw(const Sprite2D& sprite, ID3D11DeviceContext* context);

	static ObjectBuffer BuildConstantObjectBuffer(const Transform& transform);
	static MaterialBuffer BuildConstantMaterialBuffer(const Material& material);
};

#endif
