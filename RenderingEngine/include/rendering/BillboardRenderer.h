#ifndef BILLBOARD_RENDERER_H
#define BILLBOARD_RENDERER_H

#include <d3d11.h>

#include "buffers/constantBuffers/BillboardBuffer.h"
#include "buffers/constantBuffers/ConstantBuffer.h"
#include "graphics/Billboard.h"
#include <DirectXMath.h>

struct BaseCameraData;

class BillboardRenderer
{
public:
#ifndef NDEBUG
	static constexpr size_t MAX_BILLBOARDS = 1'500;
#else
	static constexpr size_t MAX_BILLBOARDS = 22'000;
#endif

	explicit BillboardRenderer(ID3D11Device* device);

	void Render(Billboard& billboard, ID3D11DeviceContext* context, const DirectX::XMMATRIX& worldMatrix);
	void RenderWithInstancing(Billboard& billboard, ID3D11DeviceContext* context, const std::vector<BillboardData>& worldMatrices);
	void UpdateCameraData(const BaseCameraData& baseCameraData, ID3D11DeviceContext* context, ID3D11ShaderResourceView* billboardSRV);

private:
	static constexpr int billboardCameraCbRegisterNumber = 0;
	static constexpr int billboardCbRegisterNumber = 1;

	BillboardCameraBuffer billboardCameraBuffer{};
	BillboardBuffer billboardBuffer{};

	ConstantBuffer<BillboardCameraBuffer> billboardCameraConstantBuffer;
	ConstantBuffer<BillboardBuffer> billboardConstantBuffer;

	// For instancing
	ComPtr<ID3D11Buffer> billboardWorldBuffer;
	ComPtr<ID3D11ShaderResourceView> billboardWorldSRV;

	static void Draw(const Billboard& billboard, ID3D11DeviceContext* context);
};

#endif