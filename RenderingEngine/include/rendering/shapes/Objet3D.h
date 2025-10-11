#pragma once

class CObjet3D
{
public:
	virtual ~CObjet3D() = default;

	virtual void Anime(float tempEcoule)
	{
	}

	virtual void Draw(const DirectX::XMMATRIX& viewProj) = 0;
};
