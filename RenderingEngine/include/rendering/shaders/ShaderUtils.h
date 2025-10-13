#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include "rendering/utils/ComPtr.h"
#include "rendering/utils/DDSTextureLoader.h"

namespace ShaderUtils
{
	ComPtr<ID3DBlob> Compile(const std::wstring& filePath,
		const std::string& entry,
		const std::string& profile);
}

#endif