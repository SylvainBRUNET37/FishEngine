#pragma once

#include "Objet3D.h"
#include "rendering/device/Device.h"

	// Classe : CBloc
	// BUT : Classe de bloc
	class CBloc : public CObjet3D
	{
	public:
		CBloc(const DirectX::XMMATRIX& matWorld, float dx, float dy, float dz, Device* pDispositif_);

		~CBloc() override;

		void Anime(float tempsEcoule) override;

		void Draw(const DirectX::XMMATRIX& viewProj) override;

	private:
		Device* pDispositif;
		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		ID3D11VertexShader* pVertexShader;
		ID3D11InputLayout* pVertexLayout;
		ID3D11Buffer* pConstantBuffer;
		ID3D11PixelShader* pPixelShader;
		DirectX::XMMATRIX matWorld;

		float rotation;

		void InitShaders();
	};
