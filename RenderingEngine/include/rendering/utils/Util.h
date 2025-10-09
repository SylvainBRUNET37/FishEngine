#ifndef UTIL_H
#define UTIL_H

template <class Type>
void DXEssayer(const Type& Resultat)
{
	if (Resultat != S_OK)
	{
		throw Resultat;
	}
}

template <class Type1, class Type2>
void DXEssayer(const Type1& Resultat, const Type2& unCode)
{
	if (Resultat != S_OK)
	{
		throw unCode;
	}
}

template <class Type>
void DXValider(const void* UnPointeur, const Type& unCode)
{
	if (UnPointeur == nullptr)
	{
		throw unCode;
	}
}

template <class Type>
void DXRelacher(Type& UnPointeur)
{
	if (UnPointeur != nullptr)
	{
		UnPointeur->Release();
		UnPointeur = nullptr;
	}
}

#endif
