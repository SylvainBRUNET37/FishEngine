#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "ShaderProgram.h"
#include "utils/ComPtr.h"

class ShaderManager
{
public:
	static ShaderProgram CreateShader(ID3D11Device* device)
	{
		ShaderProgram shaderProgram;

		const auto vsBlob = CompileShader(L"shaders/MiniPhongVS.hlsl", "MiniPhongVS", "vs_5_0");
		const auto psBlob = CompileShader(L"shaders/MiniPhongPS.hlsl", "MiniPhongPS", "ps_5_0");

		HRESULT hr = device->CreateVertexShader(vsBlob->GetBufferPointer(),
		                                        vsBlob->GetBufferSize(), nullptr,
		                                        &shaderProgram.vertexShader);
		assert(SUCCEEDED(hr));
		hr = device->CreatePixelShader(psBlob->GetBufferPointer(),
		                               psBlob->GetBufferSize(), nullptr, &shaderProgram.pixelShader);
		assert(SUCCEEDED(hr));

		// Input layout (position, normal, texcoord)
		D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		hr = device->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc),
		                               vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
		                               &shaderProgram.inputLayout);
		assert(SUCCEEDED(hr));

		return shaderProgram;
	}

private:
	// Helper: compile shader from file
	static ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const char* entry, const char* target)
	{
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
#endif
		ComPtr<ID3DBlob> byteCode;
		ComPtr<ID3DBlob> errors;
		const HRESULT hr = D3DCompileFromFile(filename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		                                      entry, target, flags, 0, &byteCode, &errors);
		if (errors) OutputDebugStringA(static_cast<char*>(errors->GetBufferPointer()));
		assert(SUCCEEDED(hr));
		return byteCode;
	}
};

#endif
