#include "pch.h"
#include "rendering/Renderer.h"

using namespace DirectX;

void Renderer::Render(const Mesh& mesh, ID3D11DeviceContext* context, const Transform& transform,
                      const SceneData& scene)
{
	auto& material = materials[mesh.materialIndex];

	// Update material constant buffer
	const auto cbMaterialParams = BuildMaterialConstantBufferParams(material);
	material.constantBuffer.Update(context, cbMaterialParams);
	material.constantBuffer.Bind(context);
	material.shaderProgram.Bind(context);

	// Update object constant buffer
	const auto cbObjectParams = BuildObjectConstantBufferParams(transform);
	objectConstantBuffer.Update(context, cbObjectParams);
	objectConstantBuffer.Bind(context);

	// Update frame constant buffer
	const auto cbFrameParams = BuildFrameConstantBufferParams(scene);
	frameConstantBuffer.Update(context, cbFrameParams);
	frameConstantBuffer.Bind(context);

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

Renderer::FrameBufferData Renderer::BuildFrameConstantBufferParams(const SceneData& sceneData)
{
	FrameBufferData params;

	params.matViewProj = XMMatrixTranspose(sceneData.matViewProj);
	params.vLumiere = sceneData.lightPosition;
	params.vCamera = sceneData.cameraPosition;
	params.vDEcl = sceneData.vDEcl;
	params.vAEcl = sceneData.vAEcl;
	params.vSEcl = sceneData.vSEcl;

	return params;
}

Renderer::ObjectConstants Renderer::BuildObjectConstantBufferParams(const Transform& transform)
{
	ObjectConstants params;

	params.matWorld = XMMatrixTranspose(transform.world);

	return params;
}

Material::MaterialBufferData Renderer::BuildMaterialConstantBufferParams(const Material& material)
{
	Material::MaterialBufferData params;

	params.vAMat = material.ambient;
	params.vDMat = material.diffuse;
	params.vSMat = material.specular;
	params.puissance = material.shininess;
	params.bTex = material.texture != nullptr;
	params.padding = XMFLOAT2(0, 0);

	return params;
}
