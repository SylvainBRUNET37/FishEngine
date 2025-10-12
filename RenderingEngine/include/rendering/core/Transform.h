#ifndef TRANSFORM_H
#define TRANSFORM_H

struct Transform
{
	DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
};

#endif