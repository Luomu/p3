#include "p3/Graphic.h"
#include "graphics/TextureBuilder.h"

namespace p3
{

using Graphics::VertexArray;
using Graphics::RenderState;
using Graphics::Material;

VertexArray* s_laserVertices = nullptr;
RenderState* s_laserState    = nullptr;
Material*    s_laserMaterial = nullptr;

Graphic::Graphic(Graphics::Renderer* r)
	: m_renderer(r)
{
}

Graphic::~Graphic()
{
}

void LaserBoltGraphic::InitResources(Graphics::Renderer* r)
{
	SDL_assert(s_laserVertices == nullptr);

	Graphics::MaterialDescriptor desc;
	desc.textures = 1;
	s_laserMaterial = r->CreateMaterial(desc);
	s_laserMaterial->texture0 = Graphics::TextureBuilder::Billboard("textures/halo_red.png").GetOrCreateTexture(r, "billboard");

	Graphics::RenderStateDesc rsd;
	rsd.blendMode = Graphics::BLEND_ALPHA_ONE;
	rsd.depthWrite = false;
	rsd.cullMode = Graphics::CULL_NONE;
	s_laserState = r->CreateRenderState(rsd);

	//zero at projectile position
	//+x down
	//+y right
	//+z forwards (or projectile direction)
	const float w = 0.5f;

	vector3f one(0.f, -w, 0.f); //top left
	vector3f two(0.f,  w, 0.f); //top right
	vector3f three(0.f,  w, -1.f); //bottom right
	vector3f four(0.f, -w, -1.f); //bottom left

	//uv coords
	const vector2f topLeft(0.f, 1.f);
	const vector2f topRight(1.f, 1.f);
	const vector2f botLeft(0.f, 0.f);
	const vector2f botRight(1.f, 0.f);

	s_laserVertices = new VertexArray(Graphics::ATTRIB_POSITION | Graphics::ATTRIB_UV0);

	//add four intersecting planes to create a volumetric effect
	for (int i=0; i < 4; i++) {
		s_laserVertices->Add(one, topLeft);
		s_laserVertices->Add(two, topRight);
		s_laserVertices->Add(three, botRight);

		s_laserVertices->Add(three, botRight);
		s_laserVertices->Add(four, botLeft);
		s_laserVertices->Add(one, topLeft);

		one.ArbRotate(vector3f(0.f, 0.f, 1.f), DEG2RAD(45.f));
		two.ArbRotate(vector3f(0.f, 0.f, 1.f), DEG2RAD(45.f));
		three.ArbRotate(vector3f(0.f, 0.f, 1.f), DEG2RAD(45.f));
		four.ArbRotate(vector3f(0.f, 0.f, 1.f), DEG2RAD(45.f));
	}

	//should register uninit function here
}

void ModelGraphic::Render()
{
	static matrix4x4f vmf;
	matrix4x4dtof(m_renderer->GetViewMatrix() * modelTransform, vmf);
	model->Render(vmf);
}

VertexArrayGraphic::VertexArrayGraphic(Graphics::Renderer* r)
	: Graphic(r)
	, vertexArray(nullptr)
	, renderState(nullptr)
	, material(nullptr)
{
}

void VertexArrayGraphic::Render()
{
	m_renderer->SetTransform(m_renderer->GetViewMatrix() * modelTransform);
	m_renderer->DrawTriangles(vertexArray, renderState, material);
}

LaserBoltGraphic::LaserBoltGraphic(Graphics::Renderer* r)
	: VertexArrayGraphic(r)
{
	vertexArray = s_laserVertices;
	renderState = s_laserState;
	material    = s_laserMaterial;
}

}
