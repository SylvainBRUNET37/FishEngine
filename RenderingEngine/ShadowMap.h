#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include <d3d11.h>

//Move this to the appropriate folder later

//Reference: Frank D. Luna
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

	ID3D11ShaderResourceView* depthMapSRV;
	ID3D11DepthStencilView* depthMapDSV;

	D3D11_VIEWPORT viewport;
};
#endif // !SHADOW_MAP_H