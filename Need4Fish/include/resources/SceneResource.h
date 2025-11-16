#ifndef SCENE_RESOURCE_H
#define SCENE_RESOURCE_H

#include <string>
#include <vector>

#include "Node.h"
#include "rendering/graphics/Material.h"
#include "rendering/graphics/Mesh.h"
#include "rendering/graphics/lights/DirectionalLight.h"
#include "rendering/graphics/lights/PointLight.h"

struct SceneResource
{
    std::string name;
    std::vector<Mesh> meshes;
    std::vector<Node> nodes;
    std::vector<Material> materials;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
};

#endif