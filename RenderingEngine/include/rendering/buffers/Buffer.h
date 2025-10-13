#ifndef BUFFER_H
#define BUFFER_H

#include "rendering/utils/ComPtr.h"

class Buffer
{
public:
	[[nodiscard]] ComPtr<ID3D11Buffer> Get() const { return buffer; }

protected:
	ComPtr<ID3D11Buffer> buffer;
};

#endif
