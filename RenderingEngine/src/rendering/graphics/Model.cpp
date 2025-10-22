#include "pch.h"
#include "rendering/graphics/Model.h"

using namespace DirectX;

Model::Model(std::vector<Mesh>&& meshes, std::vector<Material>&& materials)
	: meshes{std::move(meshes)},
	  materials{std::move(materials)}
{
}
