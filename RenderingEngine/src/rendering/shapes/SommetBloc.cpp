#include "pch.h"
#include "rendering/shapes/SommetBloc.h"

PM3D::CSommetBloc::CSommetBloc(const DirectX::XMFLOAT3 _position, const DirectX::XMFLOAT3 _normal)
{
	m_Position = _position;
	m_Normal = _normal;
}
