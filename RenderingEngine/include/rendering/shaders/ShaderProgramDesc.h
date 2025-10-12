#ifndef SHADER_PROGRAM_DESC_H
#define SHADER_PROGRAM_DESC_H

#include <unordered_set>

#include "ShaderDesc.h"

struct ShaderProgramDesc
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;
	std::unordered_set<ShaderDesc> shaderDescs;
};

#endif
