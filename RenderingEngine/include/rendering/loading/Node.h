#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

#include "rendering/core/Transform.h"
#include "rendering/graphics/Material.h"
#include "rendering/graphics/Mesh.h"

struct Node
{
    std::string name;
    uint32_t meshIndex = UINT32_MAX;
    uint32_t parentIndex = UINT32_MAX;
    std::vector<uint32_t> children;

    Transform transform;
};

struct SceneResource
{
    std::string name;
    std::vector<Mesh> meshes;
    std::vector<Node> nodes;
    std::vector<Material> materials;
};

#endif