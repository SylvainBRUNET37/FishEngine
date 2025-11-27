#include "pch.h"
#include "rendering/shaders/ShaderUtils.h"

#include <cassert>
#include <d3dcompiler.h>
#include <string>

#include "rendering/utils/Util.h"

ComPtr<ID3DBlob> ShaderUtils::Compile(const std::wstring& filePath, const std::string& entry,
                                      const std::string& profile)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> byteCode;
	ComPtr<ID3DBlob> errors;

	DXEssayer(D3DCompileFromFile(filePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entry.c_str(), profile.c_str(), flags, 0, &byteCode, &errors));
	if (errors)
		OutputDebugStringA(static_cast<char*>(errors->GetBufferPointer()));

	return byteCode;
}
