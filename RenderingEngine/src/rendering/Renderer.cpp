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
	constantObjectBuffer.Update(context, cbObjectParams);
	constantObjectBuffer.Bind(context);

	// Update frame constant buffer
	constantFrameBuffer.Update(context, frameBuffer);
	constantFrameBuffer.Bind(context);

	// Bind material's texture of the mesh
	context->PSSetShaderResources(0, 1, &material.texture);

	Draw(mesh, context);
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
