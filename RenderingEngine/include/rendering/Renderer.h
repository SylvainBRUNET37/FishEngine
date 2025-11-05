#ifndef RENDERER_H
#define RENDERER_H

#include "buffers/constantBuffers/FrameBuffer.h"
#include "buffers/constantBuffers/ObjectBuffer.h"
#include "core/SceneData.h"
#include "core/Transform.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"

class Renderer
{
public:
	explicit Renderer(ID3D11Device* device, std::vector<Material>&& materials, const int frameCbRegisterNumber,
	                  const int objectCbRegisterNumber)
		: materials{std::move(materials)}, constantFrameBuffer{device, frameCbRegisterNumber},
		  constantObjectBuffer{device, objectCbRegisterNumber}
	{
	}

	void UpdateFrameBuffer(const FrameBuffer& frameBuffer_) { frameBuffer = frameBuffer_; };
	void Render(const Mesh& mesh, ID3D11DeviceContext* context,
	          const Transform& transform);

private:
	std::vector<Material> materials;
	FrameBuffer frameBuffer{};

	ConstantBuffer<FrameBuffer> constantFrameBuffer;
	ConstantBuffer<ObjectBuffer> constantObjectBuffer;

	static void Draw(const Mesh& mesh, ID3D11DeviceContext* context);

	static ObjectBuffer BuildConstantObjectBuffer(const Transform& transform);
	static MaterialBuffer BuildConstantMaterialBuffer(const Material& material);
};

#endif
