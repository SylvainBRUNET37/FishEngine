#ifndef SHADER_REFLECTOR_H
#define SHADER_REFLECTOR_H

#include <d3d11shader.h>
#include <dxgiformat.h>

#include "rendering/device/DeviceBuilder.h"

class ShaderReflector
{
public:
	static ID3D11InputLayout* CreateInputLayout(ID3D11Device* device, ID3DBlob* shaderBlob);

private:
    static DXGI_FORMAT DetermineElemFormat(const D3D11_SIGNATURE_PARAMETER_DESC& shaderParamDesc);
    static D3D11_INPUT_ELEMENT_DESC CreateParamElemDesc(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc, DXGI_FORMAT format);
};

#endif
