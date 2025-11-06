#include "pch.h"
#include "rendering/Renderer.h"

using namespace DirectX;

void Renderer::Render(const Mesh& mesh,
                      ID3D11DeviceContext* context,
                      const Transform& transform)
{
	auto& material = materials[mesh.materialIndex];

	// Update material constant buffer
	const auto cbMaterialParams = BuildConstantMaterialBuffer(material);
	material.constantBuffer.Update(context, cbMaterialParams);
	material.constantBuffer.Bind(context);
	material.shaderProgram.Bind(context);

	// Update object constant buffer
	const auto cbObjectParams = BuildConstantObjectBuffer(transform);
	objectConstantBuffer.Update(context, cbObjectParams);
	objectConstantBuffer.Bind(context);

	// Update frame constant buffer
	frameConstantBuffer.Update(context, frameBuffer);
	frameConstantBuffer.Bind(context);

	// Bind material's texture of the mesh
	context->PSSetShaderResources(0, 1, &material.texture);

	Draw(mesh, context);
}

void Renderer::Render(Sprite2D& sprite, ID3D11DeviceContext* context)
{
	context->OMSetDepthStencilState(nullptr, 0); // TODO: delete

	// Orthographic projection to display the sprite in 2D "from the screne"
	const XMMATRIX matOrtho = XMMatrixOrthographicOffCenterRH
	(
		0.f, 1920.f,
		1080.f, 0.f,
		0.f, 1.f
	);

	// Update frame constant buffer
	sprite.shaderProgram.Bind(context);
	spriteConstantBuffer.Update(context, {.matProj = XMMatrixTranspose(matOrtho) });
	spriteConstantBuffer.Bind(context);

	context->PSSetShaderResources(0, 1, &sprite.texture.texture);

	Draw(sprite, context);
}

void Renderer::Draw(const Mesh& mesh, ID3D11DeviceContext* context)
{
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;

	const auto rawVertexBuffer = mesh.vertexBuffer.Get();

	// Bind vertex buffer
	context->IASetVertexBuffers(0, 1, &rawVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
}

void Renderer::Draw(const Sprite2D& sprite, ID3D11DeviceContext* context)
{
	// There is 2 triangle in a sprite, so 6 vertices
	static constexpr UINT SPRITE_VERTICES_COUNT = 6;

	static constexpr UINT stride = sizeof(VertexSprite);
	static constexpr UINT offset = 0;

	const auto rawVertexBuffer = sprite.vertexBuffer.Get();

	// Bind vertex buffer
	context->IASetVertexBuffers(0, 1, &rawVertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(SPRITE_VERTICES_COUNT, 0);
}

ObjectBuffer Renderer::BuildConstantObjectBuffer(const Transform& transform)
{
	ObjectBuffer params;

	params.matWorld = XMMatrixTranspose(transform.world);

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
