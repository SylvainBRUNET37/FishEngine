#ifndef CONSTANT_BUFFER
#define CONSTANT_BUFFER

#include "rendering/buffers/Buffer.h"
#include "rendering/utils/Util.h"
#include "string"

template <typename Params>
class ConstantBuffer : public Buffer
{
public:
	explicit ConstantBuffer(ID3D11Device* device, const int registerNumber) : registerNumber{registerNumber}
	{
		D3D11_BUFFER_DESC constantBufferDesc{};

		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.ByteWidth = static_cast<UINT>(sizeof(Params));
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		std::string debugName = "buffer-in-ConstantBuffer";
		debugName.append(typeid(Params).name());
		DXEssayer(device->CreateBuffer(&constantBufferDesc, nullptr, &buffer));
		SetDebugName(buffer, debugName);
	}

	void Update(ID3D11DeviceContext* context, const Params& params)
	{
		D3D11_MAPPED_SUBRESOURCE mapped{};
		DXEssayer(context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));

		std::memcpy(mapped.pData, &params, sizeof(Params));

		context->Unmap(buffer, 0);
	}

	void Bind(ID3D11DeviceContext* context)
	{
		context->VSSetConstantBuffers(registerNumber, 1, &buffer);
		context->PSSetConstantBuffers(registerNumber, 1, &buffer);
	}

private:
	int registerNumber; // The corresponding register number in the shader program constant buffer (b0, b1, b2, ...)
};

#endif
