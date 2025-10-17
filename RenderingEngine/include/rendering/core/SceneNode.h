#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include "rendering/graphics/Model.h"

struct SceneNode
{
    std::string name;
    DirectX::XMMATRIX transform;
    Model* model;
    std::vector<SceneNode> childrens;
};

#endif