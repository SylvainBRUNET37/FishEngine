#ifndef MAIN_ENGINE_H
#define MAIN_ENGINE_H

#include <algorithm>
#include <d3d11.h>
#include <functional>
#include <memory>

#include "rendering/device/DispositifD3D11.h"
#include "rendering/shapes/Bloc.h"
#include "rendering/shapes/Objet3D.h"
#include "rendering/utils/Horloge.h"

class MainEngine
{
public:
	using MainLoopCallback = std::function<bool()>;

	explicit MainEngine(PM3D::CDispositifD3D11* dispositif, const std::initializer_list<MainLoopCallback> callbacks)
		: mainLoopCallbacks{callbacks}, pDispositif{dispositif}
	{
		InitScene();
		InitAnimation();

		// TODO: init input system 
	}

	void AddObjectToScene(const DirectX::XMMATRIX& pos, const float dx, const float dy, const float dz)
	{
		ListeScene.emplace_back(std::make_unique<PM3D::CBloc>(pos, dx, dy, dz, pDispositif));
	}

	void Run()
	{
		auto allCallbacksSucceeded = [&]
		{
			return std::ranges::all_of(mainLoopCallbacks, [](auto& callback)
			{
				return callback();
			});
		};

		while (allCallbacksSucceeded())
		{
			Animation();
		}
	}

private:
	std::vector<MainLoopCallback> mainLoopCallbacks;

	// TODO: place everything bellow this elsewhere
	DirectX::XMMATRIX matView{};
	DirectX::XMMATRIX matProj{};
	DirectX::XMMATRIX matViewProj{};
	PM3D::CDispositifD3D11* pDispositif;

	std::vector<std::unique_ptr<PM3D::CObjet3D>> ListeScene;

	PM3D::Horloge horloge{};
	int64_t TempsSuivant{};
	int64_t TempsCompteurPrecedent{};

	void AnimeScene(const double tempsEcoule) const
	{
		for (const auto& object3D : ListeScene)
		{
			object3D->Anime(tempsEcoule);
		}
	}

	void Animation()
	{
		constexpr int IMAGESPARSECONDE = 60;
		constexpr double EcartTemps = 1.0 / static_cast<double>(IMAGESPARSECONDE);

		const int64_t TempsCompteurCourant = GetTimeSpecific();
		const double TempsEcoule =
			GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);
		// Est-il temps de rendre l’image ?
		if (TempsEcoule > EcartTemps)
		{
			// Affichage optimisé
			pDispositif->Present(); // On enlevera « // » plus tard
			// On prépare la prochaine image
			AnimeScene(TempsEcoule);
			// On rend l’image sur la surface de travail
			// (tampon d’arrière plan)
			RenderScene();
			// Calcul du temps du prochain affichage
			TempsCompteurPrecedent = TempsCompteurCourant;
		}
	}

	static int64_t GetTimeSpecific()
	{
		return PM3D::Horloge::GetTimeCount();
	}

	[[nodiscard]] double GetTimeIntervalsInSec(const int64_t start, const int64_t stop) const
	{
		return horloge.GetTimeBetweenCounts(start, stop);
	}

	void RenderScene() const
	{
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
		ID3D11RenderTargetView* pRenderTargetView = pDispositif->GetRenderTargetView();
		// On efface la surface de rendu
		constexpr float Couleur[4] = {0.0f, 0.5f, 0.0f, 1.0f}; // RGBA - Vert pour le moment
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, Couleur);

		for (const auto& object3D : ListeScene)
		{
			object3D->Draw(matViewProj);
		}
	}

	void InitAnimation()
	{
		TempsSuivant = GetTimeSpecific();
		TempsCompteurPrecedent = TempsSuivant;

		// première Image
		RenderScene();
	}

	void InitScene()
	{
		// Initialisation des matrices View et Proj
		// Dans notre cas, ces matrices sont fixes
		matView = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		                                    DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		                                    DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
		float champDeVision = DirectX::XM_PI / 4; // 45 degrés
		float ratioDAspect = 1.0; // horrible, il faudra corriger ça

		float planRapproche = 2.0;
		float planEloigne = 20.0;
		matProj = DirectX::XMMatrixPerspectiveFovLH(
			champDeVision,
			ratioDAspect,
			planRapproche,
			planEloigne);
		// Calcul de VP à l’avance
		matViewProj = matView * matProj;
	}
};

#endif
