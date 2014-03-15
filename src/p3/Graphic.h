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

//has a vtx array, render state and a material
class VertexArrayGraphic : public Graphic
{
public:
	VertexArrayGraphic(Graphics::Renderer*);
	virtual void Render() override;

	Graphics::VertexArray* vertexArray;
	Graphics::RenderState* renderState;
	Graphics::Material* material;

	matrix4x4d renderTransform;
};

class LaserBoltGraphic : public VertexArrayGraphic
{
public:
	LaserBoltGraphic(Graphics::Renderer*);

	static void InitResources(Graphics::Renderer*);
};

}
