#include "pch.h"
#include "rendering/shaders/ShaderBuilder.h"

ShaderProgram ShaderBuilder::CreateShaderProgram(ID3D11Device* device, const ShaderProgramDesc& shaderProgramDesc)
{
	ShaderProgram shaderProgram;

	for (auto& shaderDesc : shaderProgramDesc.shaderDescs)
	{
		HRESULT result;
		const auto shaderBlob = CompileShader(shaderDesc.path, shaderDesc.entryPoint, shaderDesc.profile);

		// Create the corresponding shader
		switch (shaderDesc.stage) // TODO: avoid the big switch case if needed)
		{
		case ShaderStage::Vertex:
			result = device->CreateVertexShader(shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(), nullptr,
				&shaderProgram.vertexShader);
			assert(SUCCEEDED(result));

			result = device->CreateInputLayout(shaderProgramDesc.layoutDesc.data(), shaderProgramDesc.layoutDesc.size(),
				shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
				&shaderProgram.inputLayout);

			assert(SUCCEEDED(result));
			break;
		case ShaderStage::Pixel:
			result = device->CreatePixelShader(shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(), nullptr, &shaderProgram.pixelShader);
			assert(SUCCEEDED(result));
			break;
		default:
			throw std::runtime_error("Shader stage is not implemented");
		}
	}

	return shaderProgram;
}

ComPtr<ID3DBlob> ShaderBuilder::CompileShader(const std::wstring& filePath,
                                              const std::string& entry,
                                              const std::string& profile)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> byteCode;
	ComPtr<ID3DBlob> errors;

	const HRESULT hr = D3DCompileFromFile(filePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
	                                      entry.c_str(), profile.c_str(), flags, 0, &byteCode, &errors);
	if (errors) 
		OutputDebugStringA(static_cast<char*>(errors->GetBufferPointer()));

	assert(SUCCEEDED(hr));
	return byteCode;
}
