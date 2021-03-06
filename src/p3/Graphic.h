#pragma once
#include "graphics/Renderer.h"
#include "graphics/Material.h"
#include "graphics/VertexBuffer.h"
#include "graphics/RenderState.h"
#include "scenegraph/Model.h"

namespace p3
{
class Scene;
/**
 * Basic drawable object, register to Scene
 * (only once) for drawing.
 * Upon destruction, a Graphic unregisters itself
 * from the Scene.
 */
class Graphic : public RefCounted
{
public:
	Graphic(Graphics::Renderer* r);
	virtual ~Graphic();
	virtual void Render() = 0;

	matrix4x4d modelTransform;
	//has to be copied here due to Frame system
	matrix4x4d viewTransform;

	double depth; //for sorting

	vector3d projPos;

protected:
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
