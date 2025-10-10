#include "pch.h"
#include "rendering/device/InfoDispositif.h"

#include "rendering/utils/Util.h"

CInfoDispositif::CInfoDispositif(int NoAdaptateur)
{
	IDXGIFactory* pFactory = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIOutput* pOutput = nullptr;
	valide = false;
	// Créer un IDXGIFactory.
	DXEssayer(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory)));

	if (FAILED(pFactory->EnumAdapters(NoAdaptateur, &pAdapter))) return;
	// Obtenir les informations de la première sortie de l’adaptateur
	// (Le moniteur)
	if (FAILED(pAdapter->EnumOutputs(0, &pOutput))) return;
	// Obtenir la description de l’état courant
	DXGI_OUTPUT_DESC outDesc;
	DXEssayer(pOutput->GetDesc(&outDesc));
	largeur = outDesc.DesktopCoordinates.right - outDesc.DesktopCoordinates.left;
	hauteur = outDesc.DesktopCoordinates.bottom - outDesc.DesktopCoordinates.top;
	valide = true;
	DXGI_ADAPTER_DESC Desc;
	DXEssayer(pAdapter->GetDesc(&Desc));
	// Mémoire dédiée (en megabytes).
	memoire = static_cast<int>(Desc.DedicatedVideoMemory / 1024 / 1024);
	// Nom de la carte video.
	wcscpy_s(nomcarte, 100, Desc.Description);
	// Faire le ménage pour éviter les « memory leaks »
	DXRelacher(pOutput);
	DXRelacher(pAdapter);
	DXRelacher(pFactory);
}

CInfoDispositif::CInfoDispositif(DXGI_MODE_DESC modeDesc)
{
	// Énumération des adaptateurs
	IDXGIFactory* pFactory = nullptr;
	DXEssayer(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory)));
	IDXGIAdapter* pAdapter;
	std::vector<IDXGIAdapter*> vAdapters;
	for (UINT i = 0;
		pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		vAdapters.push_back(pAdapter);
	}
	// On travaille presque toujours avec vAdapters[0]
	// à moins d’avoir plusieurs cartes non-hybrides
	*this = CInfoDispositif(0);
	// Obtenir la sortie 0 - le moniteur principal
	IDXGIOutput* pOutput = nullptr;
	DXEssayer(vAdapters[0]->EnumOutputs(0, &pOutput));
	// Obtenir le mode le plus intéressant
	DXEssayer(pOutput->FindClosestMatchingMode(&modeDesc, &mode, nullptr));
	// Faire le ménage pour éviter les « memory leaks »
	DXRelacher(pOutput);
	for (int i = 0; i < vAdapters.size(); ++i)
	{
		DXRelacher(vAdapters[i]);
	}
	DXRelacher(pFactory);
}