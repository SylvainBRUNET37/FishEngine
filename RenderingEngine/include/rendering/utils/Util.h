#ifndef UTIL_H
#define UTIL_H

#include <string>

template <class Type>
void DXEssayer(const Type& Resultat)
{
	if (Resultat != S_OK) [[unlikely]]
	{
		throw Resultat;
	}
}

template <class Type1, class Type2>
void DXEssayer(const Type1& Resultat, const Type2& unCode)
{
	if (Resultat != S_OK) [[unlikely]]
	{
		throw unCode;
	}
}

template <class Type>
void DXValider(const void* UnPointeur, const Type& unCode)
{
	if (UnPointeur == nullptr) [[unlikely]]
	{
		throw unCode;
	}
}

template <class Type>
void DXRelacher(Type& UnPointeur)
{
	if (UnPointeur != nullptr) [[likely]]
	{
		UnPointeur->Release();
		UnPointeur = nullptr;
	}
}

//référence: https://seanmiddleditch.github.io/direct3d-11-debug-api-tricks/
inline void SetDebugName(ID3D11DeviceChild* child, const std::string& name) {
	if (child != nullptr /*&& name != null*/) {
		child->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
	}
}

inline void SetDebugName(ID3D11Device* device, const std::string& name) {
	if (device != nullptr /*&& name != null*/) {
		device->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
	}
}

inline void SetDebugName(IDXGISwapChain* swapChain, const std::string& name) {
	if (swapChain != nullptr /*&& name != null*/) {
		swapChain->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
	}
}

#endif
