#ifndef BILLBOARD_RENDERER_H
#define BILLBOARD_RENDERER_H

#include <d3d11.h>

#include "buffers/constantBuffers/BillboardBuffer.h"
#include "buffers/constantBuffers/ConstantBuffer.h"
#include "graphics/Billboard.h"

class BillboardRenderer
{
public:
	explicit BillboardRenderer(ID3D11Device* device);

	void Render(Billboard& billboard, ID3D11DeviceContext* context, const BillboardBuffer& billboardBuffer);

private:
	static constexpr int billboardCbRegisterNumber = 0;

	ConstantBuffer<BillboardBuffer> billboardConstantBuffer;

	static void Draw(const Billboard& billboard, ID3D11DeviceContext* context);
};

#endif