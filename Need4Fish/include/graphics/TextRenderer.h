#pragma once
#include <d3d11.h>
#include <Gdiplus.h>
#include <memory>
#include <string>

#include "rendering/utils/ComPtr.h"

#pragma comment(lib, "gdiplus.lib")


class TextRenderer
{
public:
	TextRenderer(ID3D11Device* pDispositif, int largeur, int hauteur,
		Gdiplus::Font* pPolice);
	~TextRenderer();
	void Ecrire(const std::wstring& s) const;
	ComPtr<ID3D11ShaderResourceView> GetTextureView() const { return pTextureView; }
	static void Init();
	static void Close();
	UINT GetTextWidth() const { return TexWidth; }
	UINT GetTextHeigth() const { return TexHeight; }

private:
	UINT TexWidth;
	UINT TexHeight;
	ID3D11Texture2D* pTexture;
	IDXGISurface1* pSurface;
	ComPtr<ID3D11ShaderResourceView> pTextureView;
	ID3D11Device* pDispo;
	Gdiplus::Font* pFont;
	std::unique_ptr<Gdiplus::Bitmap> pCharBitmap;
	std::unique_ptr<Gdiplus::Graphics> pCharGraphics;
	std::unique_ptr<Gdiplus::SolidBrush> pBlackBrush;
	// Variables statiques pour GDI+
	static ULONG_PTR token;
};
