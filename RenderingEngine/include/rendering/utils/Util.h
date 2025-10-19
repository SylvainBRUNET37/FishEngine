#ifndef UTIL_H
#define UTIL_H

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

#endif
