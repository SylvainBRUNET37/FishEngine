#ifndef MODEL_H
#define MODEL_H

#include "Material.h"
#include "Mesh.h"

struct Model
{
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<std::unique_ptr<Material>> materials;
};

#endif