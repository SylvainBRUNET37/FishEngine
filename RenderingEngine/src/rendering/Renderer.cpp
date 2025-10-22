#include "pch.h"
#include "rendering/Renderer.h"

using namespace DirectX;

void Renderer::Draw(Model& model, ID3D11DeviceContext* context, const Transform& transform, const SceneData& scene)
{
	// Prepare constant buffer once for each mesh
	for (size_t i = 0; i < model.meshes.size(); ++i)
	{
		auto& mat = model.materials[model.meshes[i].materialIndex];
		auto& mesh = model.meshes[i];
		auto params = BuildMeshConstantBufferParams(mat, transform, scene);

		mesh.shaderProgram.Bind(context);
		mesh.constantBuffer.Update(context, sizeof(decltype(params)), &params);
		mesh.constantBuffer.Bind(context);

		// Bind material's texture of the mesh
		context->PSSetShaderResources(0, 1, &mat.texture);

		Draw(model.meshes[i], context);
	}
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

Mesh::ConstantBufferParams Renderer::BuildMeshConstantBufferParams(const Material& material,
                                                                    const Transform& transform, const SceneData& scene)
{
	Mesh::ConstantBufferParams params;

	params.matWorld = XMMatrixTranspose(transform.world);
	params.matWorldViewProj = XMMatrixTranspose(transform.world * transform.view * transform.proj);
	params.vLumiere = scene.lightPosition;
	params.vCamera = scene.cameraPosition;
	params.vAEcl = scene.vAEcl;
	params.vDEcl = scene.vDEcl;
	params.vSEcl = scene.vSEcl;
	params.vAMat = material.ambient;
	params.vDMat = material.diffuse;
	params.vSMat = material.specular;
	params.puissance = material.shininess;
	params.bTex = material.texture != nullptr;
	params.remplissage = XMFLOAT2(0, 0);

	return params;
}
