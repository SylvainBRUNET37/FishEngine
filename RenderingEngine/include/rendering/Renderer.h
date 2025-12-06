#ifndef RENDERER_H
#define RENDERER_H

#include "BillboardRenderer.h"
#include "buffers/constantBuffers/FrameBuffer.h"
#include "buffers/constantBuffers/ObjectBuffer.h"
#include "buffers/constantBuffers/SpriteBuffer.h"
#include "core/Transform.h"
#include "device/RenderContext.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/Sprite2D.h"
#include "postProcessing/PostProcessSettings.h"
#include "buffers/constantBuffers/ShadowMapLightWVPBuffer.h"
#include <rendering/shaders/ShaderBank.h>

struct BaseCameraData;
struct Billboard;
struct BillboardCameraBuffer;

class Renderer
{
public:
	explicit Renderer(RenderContext* renderContext, std::vector<Material>&& materials);

	void UpdateFrameBuffer(const FrameBuffer& frameBuffer_) { frameBuffer = frameBuffer_; };
	void Render(const Mesh& mesh, ID3D11DeviceContext* context, const Transform& transform);
	void RenderWithShadowMap(const Mesh& mesh, ID3D11DeviceContext* context, const Transform& transform, const DirectX::XMMATRIX shadowTransform, ID3D11ShaderResourceView* depthMapSRV);
	void RenderToShadowMap(const Mesh& mesh, ID3D11DeviceContext* context, const Transform& transform, DirectX::XMMATRIX lightView, DirectX::XMMATRIX lightProjection, ShaderBank& shaderBank);
	void Render(Sprite2D& sprite, ID3D11DeviceContext* context) const;
	void Render(Billboard& billboard, const DirectX::XMMATRIX& worldMatrix, const BaseCameraData& baseCameraData);
	void RenderWithInstancing(Billboard& billboard, const std::vector<BillboardData>& worldMatrices, const BaseCameraData& baseCameraData);
	void RenderPostProcess(ID3D11VertexShader* postProcessVertexShader, 
		                   ID3D11PixelShader* postProcessPixelShader,
	                       const PostProcessSettings& parameters);
	void UpdateScene() const;
	void PrepareSceneForDistortion() const;
	void PrepareSceneForBillboard();
	void PrepareSceneForSprite();
	void ClearPixelShaderResources();

private:
	static constexpr int frameCbRegisterNumber = 0;
	static constexpr int objectCbRegisterNumber = 1;
	static constexpr int spriteCbRegisterNumber = 0;
	static constexpr int postProcessCbRegisterNumber = 0;
	static constexpr int shadowMapLightWVPCbRegisterNumber = 0;

	RenderContext* renderContext;
	ComPtr<ID3D11SamplerState> textureSampler;
	ComPtr<ID3D11SamplerState> causticSampler;
	ComPtr<ID3D11SamplerState> shadowMapSamplerComparisonState;

	BillboardRenderer billboardRenderer;

	std::vector<Material> materials;
	FrameBuffer frameBuffer{};

	ConstantBuffer<FrameBuffer> frameConstantBuffer;
	ConstantBuffer<ObjectBuffer> objectConstantBuffer;
	ConstantBuffer<SpriteBuffer> spriteConstantBuffer;
	ConstantBuffer<PostProcessSettings> postProcessSettingsBuffer;
	ConstantBuffer<ShadowMapLightWVPBuffer> shadowMapLightWVPBuffer;

	Texture causticTexture;

	void Draw(const Mesh& mesh) const;
	void DrawToShadowMap(const Mesh& mesh) const;
	void DoubleSidedDraw(const Mesh& mesh) const;
	void Draw(const Sprite2D& sprite) const;

	static ObjectBuffer BuildConstantObjectBuffer(const Transform& transform);
	static ObjectBuffer BuildConstantObjectBuffer(const Transform& transform, const DirectX::XMMATRIX shadowTransform);
	static MaterialBuffer BuildConstantMaterialBuffer(const Material& material);
	static ShadowMapLightWVPBuffer BuildConstantShadowMapLightWVPBuffer(const Transform& transformForWorldMatrix, const DirectX::XMMATRIX lightViewMatrix, const DirectX::XMMATRIX lightProjectionMatrix);
};

#endif
