#include "pch.h"
#include "rendering/shapes/Bloc.h"

#include "rendering/device/DispositifD3D11.h"
#include "rendering/shapes/SommetBloc.h"
#include "rendering/utils/Util.h"

using namespace DirectX;
using namespace std;

namespace
{
	constexpr int RENDERING_ERROR_CODE = -1;
}

namespace PM3D
{
	const uint16_t index_bloc[36] = {
		0, 1, 2, // devant
		0, 2, 3, // devant
		5, 6, 7, // arri�re
		5, 7, 4, // arri�re
		8, 9, 10, // dessous
		8, 10, 11, // dessous
		13, 14, 15, // dessus
		13, 15, 12, // dessus
		19, 16, 17, // gauche
		19, 17, 18, // gauche
		20, 21, 22, // droite
		20, 22, 23 // droite
	};

	// FONCTION : CBloc, constructeur param�tr�
	// BUT : Constructeur d�une classe de bloc
	// PARAM�TRES:
	// dx, dy, dz:dimension en x, y, et z
	// pDispositif: pointeur sur notre objet dispositif
	CBloc::CBloc(const XMMATRIX& matWorld, const float dx, const float dy, const float dz, CDispositifD3D11* pDispositif_)
		: pDispositif(pDispositif_), matWorld{ matWorld }, rotation(0.0f)
	{
		// Les points
		const XMFLOAT3 point[8] =
		{
			XMFLOAT3(-dx / 2, dy / 2, -dz / 2),
			XMFLOAT3(dx / 2, dy / 2, -dz / 2),
			XMFLOAT3(dx / 2, -dy / 2, -dz / 2),
			XMFLOAT3(-dx / 2, -dy / 2, -dz / 2),
			XMFLOAT3(-dx / 2, dy / 2, dz / 2),
			XMFLOAT3(-dx / 2, -dy / 2, dz / 2),
			XMFLOAT3(dx / 2, -dy / 2, dz / 2),
			XMFLOAT3(dx / 2, dy / 2, dz / 2)
		};

		// Calculer les normales
		constexpr XMFLOAT3 n0(0.0f, 0.0f, -1.0f); // devant
		constexpr XMFLOAT3 n1(0.0f, 0.0f, 1.0f); // arri�re
		constexpr XMFLOAT3 n2(0.0f, -1.0f, 0.0f); // dessous
		constexpr XMFLOAT3 n3(0.0f, 1.0f, 0.0f); // dessus
		constexpr XMFLOAT3 n4(-1.0f, 0.0f, 0.0f); // face gauche
		constexpr XMFLOAT3 n5(1.0f, 0.0f, 0.0f); // face droite

		CSommetBloc sommets[24];
		// Le devant du bloc
		sommets[0] = CSommetBloc(point[0], n0);
		sommets[1] = CSommetBloc(point[1], n0);
		sommets[2] = CSommetBloc(point[2], n0);
		sommets[3] = CSommetBloc(point[3], n0);
		// L�arri�re du bloc
		sommets[4] = CSommetBloc(point[4], n1);
		sommets[5] = CSommetBloc(point[5], n1);
		sommets[6] = CSommetBloc(point[6], n1);
		sommets[7] = CSommetBloc(point[7], n1);
		// Le dessous du bloc
		sommets[8] = CSommetBloc(point[3], n2);
		sommets[9] = CSommetBloc(point[2], n2);
		sommets[10] = CSommetBloc(point[6], n2);
		sommets[11] = CSommetBloc(point[5], n2);
		// Le dessus du bloc
		sommets[12] = CSommetBloc(point[0], n3);
		sommets[13] = CSommetBloc(point[4], n3);
		sommets[14] = CSommetBloc(point[7], n3);
		sommets[15] = CSommetBloc(point[1], n3);
		// La face gauche
		sommets[16] = CSommetBloc(point[0], n4);
		sommets[17] = CSommetBloc(point[3], n4);
		sommets[18] = CSommetBloc(point[5], n4);
		sommets[19] = CSommetBloc(point[4], n4);
		// La face droite
		sommets[20] = CSommetBloc(point[1], n5);
		sommets[21] = CSommetBloc(point[7], n5);
		sommets[22] = CSommetBloc(point[6], n5);
		sommets[23] = CSommetBloc(point[2], n5);

		// Cr�ation du vertex buffer et copie des sommets
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(sommets);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = sommets;
		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer),
			RENDERING_ERROR_CODE);

		// Cr�ation de l�index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(index_bloc);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = index_bloc;
		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			RENDERING_ERROR_CODE);

		InitShaders();
	}

	CBloc::~CBloc()
	{
		DXRelacher(pPixelShader);
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pVertexShader);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	};

	void CBloc::Anime(const float tempsEcoule)
	{
		rotation = rotation + ((XM_PI * 2.0f) / 3.0f * tempsEcoule);

		// modifier la matrice de l�objet bloc
		matWorld = XMMatrixRotationX(rotation);
	}

	void CBloc::Draw(const XMMATRIX& viewProj)
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Source des sommets
		const UINT stride = sizeof(CSommetBloc);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		// Source des index
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);
		// Activer le VS
		pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);
		// Initialiser et s�lectionner les � constantes � du VS
		const XMMATRIX matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr,
			&matWorldViewProj, 0, 0);
		pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
		// Activer le PS
		pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);
		// **** Rendu de l�objet
		pImmediateContext->DrawIndexed(ARRAYSIZE(index_bloc), 0, 0);
	}

	void CBloc::InitShaders()
	{
		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();
		ID3DBlob* pVSBlob = nullptr;
		DXEssayer(D3DCompileFromFile(L"shaders/VS1.hlsl",
			nullptr, nullptr,
			"VS1",
			"vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&pVSBlob, nullptr), RENDERING_ERROR_CODE);
		DXEssayer(pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
			pVSBlob->GetBufferSize(),
			nullptr,
			&pVertexShader),
			RENDERING_ERROR_CODE);

		// Cr�er l�organisation des sommets
		pVertexLayout = nullptr;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
			CSommetBloc::numElements,
			pVSBlob->GetBufferPointer(),
			pVSBlob->GetBufferSize(),
			&pVertexLayout),
			RENDERING_ERROR_CODE);
		pVSBlob->Release(); // On n�a plus besoin du blob

		// Cr�ation d�un tampon pour les constantes du VS
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(matWorld);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		// Compilation et chargement du pixel shader
		ID3DBlob* pPSBlob = nullptr;
		DXEssayer(D3DCompileFromFile(L"shaders/PS1.hlsl",
			nullptr, nullptr,
			"PS1",
			"ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&pPSBlob,
			nullptr), RENDERING_ERROR_CODE);
		DXEssayer(pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
			pPSBlob->GetBufferSize(),
			nullptr,
			&pPixelShader),
			RENDERING_ERROR_CODE);
		pPSBlob->Release(); // On n�a plus besoin du blob
	}
}
