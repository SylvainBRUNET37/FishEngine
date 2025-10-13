#pragma once
#include "rendering/utils/ComPtr.h"

namespace ShaderUtils
{
	inline ComPtr<ID3DBlob> CompileShader(const std::wstring& filePath,
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
}
