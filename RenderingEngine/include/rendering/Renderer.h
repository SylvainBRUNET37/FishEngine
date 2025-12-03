#ifndef RENDERER_H
#define RENDERER_H

#include "buffers/constantBuffers/FrameBuffer.h"
#include "buffers/constantBuffers/ObjectBuffer.h"
#include "buffers/constantBuffers/SpriteBuffer.h"
#include "core/Transform.h"
#include "device/RenderContext.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/Sprite2D.h"
#include "postProcessing/PostProcessSettings.h"

class Renderer
{
public:
	explicit Renderer(RenderContext* renderContext, std::vector<Material>&& materials);

	void UpdateFrameBuffer(const FrameBuffer& frameBuffer_) { frameBuffer = frameBuffer_; };
	void Render(const Mesh& mesh, ID3D11DeviceContext* context, const Transform& transform);
	void Render(Sprite2D& sprite, ID3D11DeviceContext* context);
	void RenderPostProcess(ID3D11VertexShader* postProcessVertexShader, 
		                   ID3D11PixelShader* postProcessPixelShader,
	                       const PostProcessSettings& parameters);
	void ClearPixelShaderResources();
	void RenderScene() const;

private:
	static constexpr int frameCbRegisterNumber = 0;
	static constexpr int objectCbRegisterNumber = 1;
	static constexpr int spriteCbRegisterNumber = 0;
	static constexpr int postProcessCbRegisterNumber = 0;

	RenderContext* renderContext;
	ComPtr<ID3D11SamplerState> textureSampler;
	ComPtr<ID3D11SamplerState> causticSampler;

	std::vector<Material> materials;
	FrameBuffer frameBuffer{};

	ConstantBuffer<FrameBuffer> frameConstantBuffer;
	ConstantBuffer<ObjectBuffer> objectConstantBuffer;
	ConstantBuffer<SpriteBuffer> spriteConstantBuffer;
	ConstantBuffer<PostProcessSettings> postProcessSettingsBuffer;

	Texture causticTexture;

	void Draw(const Mesh& mesh) const;
	void DoubleSidedDraw(const Mesh& mesh) const;
	void Draw(const Sprite2D& sprite) const;

	static ObjectBuffer BuildConstantObjectBuffer(const Transform& transform);
	static MaterialBuffer BuildConstantMaterialBuffer(const Material& material);
};

#endif
