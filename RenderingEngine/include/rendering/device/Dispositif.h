#ifndef DOSPOSITIF_H
#define DOSPOSITIF_H

namespace PM3D
{
	enum CDS_MODE
	{
		CDS_FENETRE,
		CDS_PLEIN_ECRAN
	};

	class CDispositif
	{
	public:
		virtual ~CDispositif() = default;
		virtual void Present() { PresentSpecific(); };
		virtual void PresentSpecific() = 0;
	};
}

#endif
