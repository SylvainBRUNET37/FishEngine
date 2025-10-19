#ifndef INFO_DEVICE_H
#define INFO_DEVICE_H

class CInfoDispositif
{
public:
	enum INFODISPO_TYPE
	{
		ADAPTATEUR_COURANT
	};

	explicit CInfoDispositif(int NoAdaptateur);
	explicit CInfoDispositif(DXGI_MODE_DESC modeDesc);

	bool EstValide() const { return valide; }
	int GetLargeur() const { return largeur; }
	int GetHauteur() const { return hauteur; }
	int GetMemoire() const { return memoire; }
	const wchar_t* GetNomCarte() const { return nomcarte; }
	void GetDesc(DXGI_MODE_DESC& modeDesc) { modeDesc = mode; }

private:
	bool valide;
	int largeur;
	int hauteur;
	int memoire;
	wchar_t nomcarte[100];
	DXGI_MODE_DESC mode;
};

#endif
