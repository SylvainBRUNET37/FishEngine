#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include "ShaderDesc.h"
#include "ShaderProgram.h"
#include "ShaderProgramDesc.h"
#include "rendering/utils/ComPtr.h"

class ShaderBuilder
{
public:
	static ShaderProgram CreateShaderProgram(ID3D11Device* device, const ShaderProgramDesc& shaderProgramDesc);

private:
	static ComPtr<ID3DBlob> CompileShader(const std::wstring& filePath,
	                                      const std::string& entry,
	                                      const std::string& profile);
};

#endif
