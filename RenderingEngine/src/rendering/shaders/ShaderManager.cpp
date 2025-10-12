#include "pch.h"
#include "rendering/shaders/ShaderManager.h"

ShaderProgram ShaderManager::CreateShaderProgram(ID3D11Device* device,
	const std::unordered_set<ShaderDesc>& shaderDescs, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc)
{
	ShaderProgram shaderProgram;

	for (auto& shaderDesc : shaderDescs)
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

			result = device->CreateInputLayout(layoutDesc.data(), layoutDesc.size(),
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

ComPtr<ID3DBlob> ShaderManager::CompileShader(const std::wstring& filePath,
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
