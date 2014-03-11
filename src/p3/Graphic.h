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
	Graphic(Graphics::Renderer* r) : m_renderer(r) { }
	virtual ~Graphic() { }
	virtual void Render() = 0;

	matrix4x4d modelTransform;
	Graphics::Renderer* m_renderer;
};

class ModelGraphic : public Graphic
{
public:
	ModelGraphic(Graphics::Renderer* r, SceneGraph::Model* m)
		: Graphic(r)
		, model(m)
	{}
	virtual void Render() override;
	SceneGraph::Model* model;
};
}
