#include "pch.h"
#include "rendering/shapes/Model.h"

using namespace DirectX;

struct alignas(16) CBParam
{
	XMMATRIX matWorldViewProj;
	XMMATRIX matWorld;
	XMFLOAT4 vLumiere;
	XMFLOAT4 vCamera;
	XMFLOAT4 vAEcl;
	XMFLOAT4 vAMat;
	XMFLOAT4 vDEcl;
	XMFLOAT4 vDMat;
	XMFLOAT4 vSEcl;
	XMFLOAT4 vSMat;
	float puissance;
	int bTex;
	XMFLOAT2 remplissage;
};


Model::Model(std::vector<Mesh>&& meshes, std::vector<Material>&& materials, GraphicsDevice* device, const ShaderProgram& shaderProgram)
	: meshes{std::move(meshes)}, materials{std::move(materials)}, matWorld(), device{device}
{
    inputLayout = shaderProgram.inputLayout;
    vertexShader = shaderProgram.vertexShader;
    pixelShader = shaderProgram.pixelShader;

    Init();
}

void Model::Draw(ID3D11DeviceContext* ctx,
    const XMMATRIX& world,
    const XMMATRIX& view,
    const XMMATRIX& proj,
    const XMFLOAT4& lightPos,
    const XMFLOAT4& cameraPos,
    const XMFLOAT4& vAEcl,
    const XMFLOAT4& vDEcl,
    const XMFLOAT4& vSEcl)
{
    ctx->VSSetShader(vertexShader, nullptr, 0);
    ctx->PSSetShader(pixelShader, nullptr, 0);
    ctx->IASetInputLayout(inputLayout);

    // Prepare CB once for each mesh (here per-mesh since material differs)
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        auto& mat = materials[meshes[i].GetMaterialIndex()];

        D3D11_MAPPED_SUBRESOURCE mapped;
        const HRESULT hr = ctx->Map(cbParam, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        assert(SUCCEEDED(hr));
        const auto cb = static_cast<CBParam*>(mapped.pData);

        XMMATRIX w = world;
        const XMMATRIX wvp = XMMatrixTranspose(w * view * proj);
        cb->matWorldViewProj = wvp;
        cb->matWorld = XMMatrixTranspose(w);
        cb->vLumiere = lightPos;
        cb->vCamera = cameraPos;
        cb->vAEcl = vAEcl;
        cb->vDEcl = vDEcl;
        cb->vSEcl = vSEcl;
        cb->vAMat = mat.ambient;
        cb->vDMat = mat.diffuse;
        cb->vSMat = mat.specular;
        cb->puissance = mat.shininess;
        cb->bTex = true;
        cb->remplissage = XMFLOAT2(0, 0);

        ctx->Unmap(cbParam, 0);

        ctx->VSSetConstantBuffers(0, 1, &cbParam);
        ctx->PSSetConstantBuffers(0, 1, &cbParam);

        if (mat.texture)
            ctx->PSSetShaderResources(0, 1, &mat.texture);
        else
        {
            ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
            ctx->PSSetShaderResources(0, 1, nullSRV);
        }

        meshes[i].Draw(ctx);
    }
}

void Model::Init()
{
    // Create constant buffer
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(CBParam);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    const HRESULT hr = device->GetD3DDevice()->CreateBuffer(&cbd, nullptr, &cbParam);
    assert(SUCCEEDED(hr));
}
