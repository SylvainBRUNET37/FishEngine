#ifndef DEPTH_STATE_H
#define DEPTH_STATE_H

#include "BlendState.h"

class DepthState
{
public:
    explicit DepthState(ID3D11Device* device);

    [[nodiscard]] ID3D11DepthStencilState* GetDepthNoWrite() const { return depthNoWrite; }
    [[nodiscard]] ID3D11DepthStencilState* GetDepthDefault() const { return depthDefault; }

private:
    ComPtr<ID3D11DepthStencilState> depthNoWrite;
    ComPtr<ID3D11DepthStencilState> depthDefault;
};

#endif