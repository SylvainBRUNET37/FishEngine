#ifndef RENDERER_H
#define RENDERER_H

#include "core/SceneData.h"
#include "core/Transform.h"
#include "graphics/Model.h"

class Renderer
{
public:
	static void Draw(Model& model, ID3D11DeviceContext* context,
	                 const Transform& transform,
	                 const SceneData& scene);

private:
	static void Draw(const Mesh& mesh, ID3D11DeviceContext* context);

	static Mesh::ConstantBufferParams BuildMeshConstantBufferParams(
		const Material& material,
		const Transform& transform,
		const SceneData& scene);
};

#endif