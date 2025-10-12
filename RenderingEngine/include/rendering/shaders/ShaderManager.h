#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <unordered_set>

#include "ShaderDesc.h"
#include "ShaderProgram.h"
#include "rendering/utils/ComPtr.h"

class ShaderManager
{
public:
	static ShaderProgram CreateShaderProgram(ID3D11Device* device,
	                                         const std::unordered_set<ShaderDesc>& shaderDescs,
	                                         const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc);

private:
	static ComPtr<ID3DBlob> CompileShader(const std::wstring& filePath,
	                                      const std::string& entry,
	                                      const std::string& profile);
};

#endif
