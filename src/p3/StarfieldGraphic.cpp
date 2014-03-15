#include "p3/StarfieldGraphic.h"

const Uint32 NUM_STARS = 2000;

#pragma pack(push, 4)
struct StarVert {
	vector3f pos;
	Color4ub col;
};
#pragma pack(pop)

namespace p3
{
StarfieldGraphic::StarfieldGraphic(Graphics::Renderer* r, Random& rng)
	: Graphic(r)
{
	Graphics::MaterialDescriptor desc;
	desc.vertexColors = true;
	m_material.Reset(m_renderer->CreateMaterial(desc));

	Graphics::VertexBufferDesc vbd;
	vbd.attrib[0].semantic = Graphics::ATTRIB_POSITION;
	vbd.attrib[0].format = Graphics::ATTRIB_FORMAT_FLOAT3;
	vbd.attrib[1].semantic = Graphics::ATTRIB_DIFFUSE;
	vbd.attrib[1].format = Graphics::ATTRIB_FORMAT_UBYTE4;
	vbd.usage = Graphics::BUFFER_USAGE_STATIC;
	vbd.numVertices = NUM_STARS;
	m_vertexBuffer.reset(m_renderer->CreateVertexBuffer(vbd));

	auto vtxPtr = m_vertexBuffer->Map<StarVert>(Graphics::BUFFER_MAP_WRITE);
	for (int i = 0; i < NUM_STARS; i++) {
		const Uint8 col = rng.Double(0.2, 0.7) * 255;

		// this is proper random distribution on a sphere's surface
		const float theta = float(rng.Double(0.0, 2.0 * M_PI));
		const float u = float(rng.Double(-1.0, 1.0));

		vtxPtr[i].pos = vector3f(
		                     1000.0f * sqrt(1.0f - u * u) * cos(theta),
		                     1000.0f * u,
		                     1000.0f * sqrt(1.0f - u * u) * sin(theta));
		vtxPtr[i].col = Color(col, col, col,	255);
	}
	m_vertexBuffer->Unmap();

	Graphics::RenderStateDesc rsd;
	rsd.depthTest  = false;
	rsd.depthWrite = false;
	m_renderState = m_renderer->CreateRenderState(rsd);
}

void StarfieldGraphic::Render()
{
	matrix4x4d vt = m_renderer->GetViewMatrix();
	vt.ClearToRotOnly();
	m_renderer->SetTransform(vt);
	m_renderer->DrawBuffer(m_vertexBuffer.get(), m_renderState, m_material.Get(), Graphics::POINTS);
}

}
