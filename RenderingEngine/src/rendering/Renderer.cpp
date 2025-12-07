#include "pch.h"
#include "rendering/Renderer.h"

#include "rendering/graphics/camera/BaseCamera.h"
#include "rendering/texture/TextureLoader.h"

#include <memory>

#include <iostream>

using namespace DirectX;

Renderer::Renderer(RenderContext* renderContext, std::vector<Material>&& materials)
	: renderContext{ renderContext }, billboardRenderer{renderContext->GetDevice()},
	  materials{std::move(materials)},
	  frameConstantBuffer{renderContext->GetDevice(), frameCbRegisterNumber},
	  objectConstantBuffer{renderContext->GetDevice(), objectCbRegisterNumber},
	  spriteConstantBuffer{renderContext->GetDevice(), spriteCbRegisterNumber},
	  shadowMapLightWVPBuffer{ renderContext->GetDevice(), shadowMapLightWVPCbRegisterNumber },
	  postProcessSettingsBuffer{renderContext->GetDevice(), postProcessCbRegisterNumber},
	  causticTexture{TextureLoader::LoadTextureFromFile("assets/textures/caustics.png", renderContext->GetDevice())}
{
	D3D11_SAMPLER_DESC textureSamplerDesc{};
	textureSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	textureSamplerDesc.MaxAnisotropy = 16;
	textureSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	textureSamplerDesc.MinLOD = 0;
	textureSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	DXEssayer(renderContext->GetDevice()->CreateSamplerState(&textureSamplerDesc, &textureSampler));

	SetDebugName(textureSampler, "textureSampler-in-Renderer");

	D3D11_SAMPLER_DESC causticSamplerDesc{};
	causticSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	causticSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	causticSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	causticSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	causticSamplerDesc.MinLOD = -FLT_MAX;
	causticSamplerDesc.MaxLOD = FLT_MAX;
	causticSamplerDesc.MipLODBias = -1.0f;
	causticSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	DXEssayer(renderContext->GetDevice()->CreateSamplerState(&causticSamplerDesc, &causticSampler));

	SetDebugName(causticSampler, "causticSampler-in-Renderer");

	D3D11_SAMPLER_DESC shadowMapSamplerComparisonStateDesc{};
	shadowMapSamplerComparisonStateDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowMapSamplerComparisonStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerComparisonStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerComparisonStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerComparisonStateDesc.BorderColor[0] = 0.0f;
	shadowMapSamplerComparisonStateDesc.BorderColor[1] = 0.0f;
	shadowMapSamplerComparisonStateDesc.BorderColor[2] = 0.0f;
	shadowMapSamplerComparisonStateDesc.BorderColor[3] = 0.0f;
	shadowMapSamplerComparisonStateDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

	DXEssayer(renderContext->GetDevice()->CreateSamplerState(&shadowMapSamplerComparisonStateDesc, &shadowMapSamplerComparisonState));

	SetDebugName(shadowMapSamplerComparisonState, "shadowMapSamplerComparisonState-in-Renderer");
}

void Renderer::Render(const Mesh& mesh,
	ID3D11DeviceContext* context,
	const Transform& transform)
{
	auto& material = materials[mesh.materialIndex];

	// Update material constant buffer
	const auto cbMaterialParams = BuildConstantMaterialBuffer(material);
	material.constantBuffer.Update(context, cbMaterialParams);
	material.constantBuffer.Bind(context);
	material.shaderProgram->Bind(context);

	// Update object constant buffer
	const auto cbObjectParams = BuildConstantObjectBuffer(transform);
	objectConstantBuffer.Update(context, cbObjectParams);
	objectConstantBuffer.Bind(context);

	// Update frame constant buffer
	frameConstantBuffer.Update(context, frameBuffer);
	frameConstantBuffer.Bind(context);

	// Bind textures and samplers
	context->PSSetShaderResources(0, 1, &material.texture);
	context->PSSetShaderResources(1, 1, &causticTexture.texture);
	context->PSSetSamplers(0, 1, &textureSampler);
	context->PSSetSamplers(1, 1, &causticSampler);

	if (material.name != "WaterMat") {
		Draw(mesh);
	}
	else {
		DoubleSidedDraw(mesh);
	}
}

void Renderer::RenderWithShadowMap(const Mesh& mesh,
                      ID3D11DeviceContext* context,
                      const Transform& transform,
					  const XMMATRIX shadowTransform,
					  ID3D11ShaderResourceView* depthMapSRV)
{
	auto& material = materials[mesh.materialIndex];

	// Update material constant buffer
	const auto cbMaterialParams = BuildConstantMaterialBuffer(material);
	material.constantBuffer.Update(context, cbMaterialParams);
	material.constantBuffer.Bind(context);
	material.shaderProgram->Bind(context);

	// Update object constant buffer
	const auto cbObjectParams = BuildConstantObjectBuffer(transform, shadowTransform);
	objectConstantBuffer.Update(context, cbObjectParams);
	objectConstantBuffer.Bind(context);

	// Update frame constant buffer
	frameConstantBuffer.Update(context, frameBuffer);
	frameConstantBuffer.Bind(context);

	// Bind textures and samplers
	context->PSSetShaderResources(0, 1, &material.texture);
	context->PSSetShaderResources(1, 1, &causticTexture.texture);
	context->PSSetShaderResources(2, 1, &depthMapSRV);
	context->PSSetSamplers(0, 1, &textureSampler);
	context->PSSetSamplers(1, 1, &causticSampler);
	context->PSSetSamplers(2, 1, &shadowMapSamplerComparisonState);

	if (material.name != "WaterMat") {
		Draw(mesh);
	}
	else {
		DoubleSidedDraw(mesh);
	}
}

void Renderer::RenderToShadowMap(const Mesh& mesh, ID3D11DeviceContext* context, const Transform& transform, DirectX::XMMATRIX lightView, DirectX::XMMATRIX lightProjection, ShaderBank& shaderBank)
{
	auto& material = materials[mesh.materialIndex];

	if (material.name == "WaterMat" || material.name == "DistortionMat" || material.name == "SkyboxMat") {
		return;
	}

	//// Update material constant buffer
	const auto cbSMParams = BuildConstantShadowMapLightWVPBuffer(transform, lightView, lightProjection);
	shadowMapLightWVPBuffer.Update(context, cbSMParams);
	shadowMapLightWVPBuffer.Bind(context);

	static const std::shared_ptr<ShaderProgram> shadowMapShader = shaderBank.GetOrCreateShaderProgram
	(
		renderContext->GetDevice(),
		"shaders/ShadowMapVS.hlsl",
		//nullptr
		"shaders/ShadowMapPS.hlsl"
	);
	shadowMapShader->Bind(context);
	context->VSSetShaderResources(0, 1, &material.texture);
	context->PSSetShaderResources(0, 1, &material.texture);

	//DrawToShadowMap(mesh);
	Draw(mesh);
}

void Renderer::Render(Sprite2D& sprite, ID3D11DeviceContext* context) const
{
	// Update frame constant buffer
	sprite.shaderProgram->Bind(context);

	context->PSSetShaderResources(0, 1, &sprite.texture.texture);

	Draw(sprite);
}

void Renderer::Render(Billboard& billboard, const XMMATRIX& worldMatrix, const BaseCameraData& baseCameraData)
{
	billboardRenderer.UpdateCameraData(baseCameraData, renderContext->GetContext(), billboard.texture.texture);
	billboardRenderer.Render(billboard, renderContext->GetContext(), worldMatrix);
}

void Renderer::RenderWithInstancing(Billboard& billboard, const std::vector<BillboardData>& worldMatrices,
	const BaseCameraData& baseCameraData)
{
	billboardRenderer.UpdateCameraData(baseCameraData, renderContext->GetContext(), billboard.texture.texture);
	billboardRenderer.RenderWithInstancing(billboard, renderContext->GetContext(), worldMatrices);
}

void Renderer::RenderPostProcess(
	ID3D11VertexShader* postProcessVertexShader,
	ID3D11PixelShader* postProcessPixelShader,
	const PostProcessSettings& parameters
)
{
	ID3D11DeviceContext* context = renderContext->GetContext();
	ID3D11RenderTargetView* renderTarget = renderContext->GetRenderTargetView();

	postProcessSettingsBuffer.Update(context, parameters);
	postProcessSettingsBuffer.Bind(context);

	ID3D11ShaderResourceView* distortionSRV =
		renderContext->GetDistortionProcess().GetShaderResourceView();

	renderContext->GetPostProcess().Draw(
		context, renderTarget, postProcessVertexShader, postProcessPixelShader, distortionSRV, renderContext->GetDepthSRV());
}

void Renderer::UpdateScene() const
{
	ID3D11DeviceContext* context = renderContext->GetContext();
	ID3D11RenderTargetView* renderTarget = renderContext->GetPostProcess().GetRenderTargetView();
	ID3D11DepthStencilView* depthStencil = renderContext->GetDepthStencilView();

	renderContext->DisableAlphaBlending();

	constexpr float backgroundColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	context->ClearRenderTargetView(renderTarget, backgroundColor);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* renderTargetViews[] = { renderTarget };
	context->OMSetRenderTargets(1, renderTargetViews, depthStencil);
}

void Renderer::PrepareSceneForDistortion() const
{
	ID3D11DeviceContext* context = renderContext->GetContext();

	ID3D11RenderTargetView* distortionRTV =
		renderContext->GetDistortionProcess().GetRenderTargetView();

	ID3D11DepthStencilView* depthStencil =
		renderContext->GetDepthStencilView();

	// Set the render target
	context->OMSetRenderTargets(1, &distortionRTV, depthStencil);

	// Clear RTV
	constexpr float clearMask[4] = { 0, 0, 0, 0 };
	context->ClearRenderTargetView(distortionRTV, clearMask);

	// Disable transparence and active depth buffer writing
	renderContext->DisableAlphaBlending();
	renderContext->EnableDefaultDepth();
}

void Renderer::PrepareSceneForBillboard()
{
	// Use a basic sampler instead of a good one
	renderContext->GetContext()->PSSetSamplers(0, 1, &causticSampler);

	renderContext->EnableAlphaBlending(); // add a better look but have a performance cost
	renderContext->EnableTransparentDepth(); // avoid artifacts by disbaling depth buffer writing
}

void Renderer::PrepareSceneForSprite()
{
	ID3D11DeviceContext* context = renderContext->GetContext();
	static const float screenWidth = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
	static const float screenHeight = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));

	// Orthographic projection to display the sprite in 2D "from the screne"
	static const XMMATRIX matOrtho = XMMatrixOrthographicOffCenterRH
	(
		0.f, screenWidth,
		screenHeight, 0.f,
		0.f, 1.f
	);

	renderContext->EnableAlphaBlending();

	spriteConstantBuffer.Update(context, { .matProj = XMMatrixTranspose(matOrtho) });
	spriteConstantBuffer.Bind(context);
}

void Renderer::Draw(const Mesh& mesh) const
{
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;

	const auto rawVertexBuffer = mesh.vertexBuffer.Get();

	// Bind vertex buffer
	renderContext->GetContext()->IASetVertexBuffers(0, 1, &rawVertexBuffer, &stride, &offset);
	renderContext->GetContext()->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	renderContext->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderContext->GetContext()->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
}

void Renderer::DrawToShadowMap(const Mesh& mesh) const
{
	renderContext->SetCullModeShadowMap();
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;

	const auto rawVertexBuffer = mesh.vertexBuffer.Get();

	// Bind vertex buffer
	renderContext->GetContext()->IASetVertexBuffers(0, 1, &rawVertexBuffer, &stride, &offset);
	renderContext->GetContext()->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	renderContext->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderContext->GetContext()->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
}

void Renderer::DoubleSidedDraw(const Mesh& mesh) const
{
	renderContext->SetCullModeCullNone();
	Draw(mesh);
	renderContext->SetCullModeCullBack();
}

void Renderer::Draw(const Sprite2D& sprite) const
{
	// There is 2 triangle in a sprite, so 6 vertices
	static constexpr UINT SPRITE_VERTICES_COUNT = 6;

	static constexpr UINT stride = sizeof(VertexSprite);
	static constexpr UINT offset = 0;

	const auto rawVertexBuffer = sprite.vertexBuffer.Get();

	// Bind vertex buffer
	renderContext->GetContext()->IASetVertexBuffers(0, 1, &rawVertexBuffer, &stride, &offset);
	renderContext->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderContext->GetContext()->Draw(SPRITE_VERTICES_COUNT, 0);
}

ObjectBuffer Renderer::BuildConstantObjectBuffer(const Transform& transform)
{
	ObjectBuffer params;

	params.matWorld = XMMatrixTranspose(transform.world);

	return params;
}

ObjectBuffer Renderer::BuildConstantObjectBuffer(const Transform& transform, const XMMATRIX shadowTransform)
{
	ObjectBuffer params;

	params.matWorld = XMMatrixTranspose(transform.world);
	params.shadowTransform = XMMatrixTranspose(transform.world * shadowTransform);

	return params;
}

MaterialBuffer Renderer::BuildConstantMaterialBuffer(const Material& material)
{
	MaterialBuffer params;

	params.vAMat = material.ambient;
	params.vDMat = material.diffuse;
	params.vSMat = material.specular;
	params.puissance = material.shininess;
	params.bTex = material.texture != nullptr;
	params.padding = XMFLOAT2(0, 0);

	return params;
}

ShadowMapLightWVPBuffer Renderer::BuildConstantShadowMapLightWVPBuffer(const Transform& transformForWorldMatrix, const DirectX::XMMATRIX lightViewMatrix, const DirectX::XMMATRIX lightProjectionMatrix)
{
	ShadowMapLightWVPBuffer param;

	XMMATRIX temp = transformForWorldMatrix.world * lightViewMatrix * lightProjectionMatrix;

	param.lightWVP = XMMatrixTranspose(temp);

	return param;
}

void Renderer::ClearPixelShaderResources() {
	ID3D11ShaderResourceView* null[] = { nullptr, nullptr };
	renderContext->GetContext()->PSSetShaderResources(0, 2, null);
}