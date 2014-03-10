#pragma once
#include "graphics/Renderer.h"
#include "graphics/Material.h"
#include "graphics/VertexBuffer.h"
#include "graphics/RenderState.h"
#include "scenegraph/Model.h"

namespace p3
{
/**
 * Basic drawable object, register to Scene
 * for drawing.
 */
class Graphic : public RefCounted
{
public:
	virtual ~Graphic() { }
	virtual void Render() = 0;

	matrix4x4f modelTransform;
};

class ModelGraphic : public Graphic
{
public:
	ModelGraphic(SceneGraph::Model* m) : model(m) {}
	virtual void Render() override;
	SceneGraph::Model* model;
};
}
