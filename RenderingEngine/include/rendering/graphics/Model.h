#ifndef MODEL_H
#define MODEL_H

#include "Material.h"
#include "Mesh.h"

struct Model
{
	explicit Model(std::vector<Mesh>&& meshes, std::vector<Material>&& materials);

	std::vector<Mesh> meshes;
	std::vector<Material> materials;
};

#endif
