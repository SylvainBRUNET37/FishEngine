#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include <d3d11.h>
#include "rendering/utils/ComPtr.h"

//Move this to the appropriate folder later

//Beaucoup du code pour les shadow maps ont pour référence la code de Frank D. Luna à l'adresse https://www.d3dcoder.net/d3d11.htm

//Note: La taille de la shadow map doit être définie dans RenderSystem.h mais aussi dans LightningPS.hlsl
class ShadowMap
{
public:
	ShadowMap(ID3D11Device* device, UINT width, UINT height);
	//~ShadowMap();

	ID3D11ShaderResourceView* DepthMapSRV();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);

private:
	//ShadowMap(const ShadowMap& rhs);
	//ShadowMap& operator=(const ShadowMap& rhs);

	UINT width;
	UINT height;

	ComPtr<ID3D11ShaderResourceView> depthMapSRV;
	ComPtr<ID3D11DepthStencilView> depthMapDSV;

	D3D11_VIEWPORT viewport;
};
#endif // !SHADOW_MAP_H