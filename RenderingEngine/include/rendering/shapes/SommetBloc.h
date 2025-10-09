#pragma once

namespace PM3D
{
	class CSommetBloc
	{
	public:
		inline static D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			}
		};
		inline static UINT numElements = ARRAYSIZE(PM3D::CSommetBloc::layout);

		CSommetBloc() = default;
		CSommetBloc(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal);

	private:
		DirectX::XMFLOAT3 m_Position;
		DirectX::XMFLOAT3 m_Normal;
	};
}
