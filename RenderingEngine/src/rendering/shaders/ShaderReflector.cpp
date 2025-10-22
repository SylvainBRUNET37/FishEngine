#include "pch.h"
#include "rendering/shaders/ShaderReflector.h"

#include "rendering/utils/ComPtr.h"
#include "rendering/utils/Util.h"

using namespace std;

DXGI_FORMAT ShaderReflector::DetermineElemFormat(const D3D11_SIGNATURE_PARAMETER_DESC& shaderParamDesc)
{
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

	// The mask field indicates how many components (x, y, z, w) are present in the parameter,
	// 1 = 1 component, 3 = 2 components, 7 = 3 components, 15 = 4 components
	if (shaderParamDesc.Mask == 1)
	{
		// Choose the format related to the type of the component
		if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32_UINT;
		else if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32_SINT;
		else if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32_FLOAT;
	}
	else if (shaderParamDesc.Mask <= 3)
	{
		if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32G32_UINT;
		else if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32G32_SINT;
		else if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32G32_FLOAT;
	}
	else if (shaderParamDesc.Mask <= 7)
	{
		if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32G32B32_UINT;
		else if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32G32B32_SINT;
		else if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32G32B32_FLOAT;
	}
	else if (shaderParamDesc.Mask <= 15)
	{
		if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32G32B32A32_UINT;
		else if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32G32B32A32_SINT;
		else if (shaderParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	return format;
}

D3D11_INPUT_ELEMENT_DESC ShaderReflector::CreateParamElemDesc(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc, const DXGI_FORMAT format)
{
	D3D11_INPUT_ELEMENT_DESC elementDesc{};

	elementDesc.SemanticName = paramDesc.SemanticName;
	elementDesc.SemanticIndex = paramDesc.SemanticIndex;
	elementDesc.Format = format;
	elementDesc.InputSlot = 0;
	elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	elementDesc.InstanceDataStepRate = 0;

	return elementDesc;
}

ID3D11InputLayout* ShaderReflector::CreateInputLayout(ID3D11Device* device, ID3DBlob* shaderBlob)
{
	
	const auto shaderBytecode = shaderBlob->GetBufferPointer();
	const auto bytecodeLength = shaderBlob->GetBufferSize();

	// Get shader reflexion
	ComPtr<ID3D11ShaderReflection> reflector;
	DXEssayer(D3DReflect(shaderBytecode, bytecodeLength, IID_ID3D11ShaderReflection, reinterpret_cast<
		                     void**>(&reflector)));

	// Get shader description (number of input/output, etc.)
	D3D11_SHADER_DESC shaderDesc;
	DXEssayer(reflector->GetDesc(&shaderDesc));

	// Use recovered data of the shader to recreate input elements description
	vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs;
	for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		DXEssayer(reflector->GetInputParameterDesc(i, &paramDesc));

		const DXGI_FORMAT elementFormat = DetermineElemFormat(paramDesc);
		const D3D11_INPUT_ELEMENT_DESC elementDesc = CreateParamElemDesc(paramDesc, elementFormat);

		inputLayoutDescs.push_back(elementDesc);
	}

	// Create the layout with informations recovered by the reflection
	ID3D11InputLayout* inputLayout = nullptr;
	DXEssayer(device->CreateInputLayout(
		inputLayoutDescs.data(),
		static_cast<UINT>(inputLayoutDescs.size()),
		shaderBytecode,
		bytecodeLength,
		&inputLayout
	));

	return inputLayout;
}
