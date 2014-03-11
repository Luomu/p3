#pragma once
#include "p3/Common.h"
#include "p3/Graphic.h"
#include "pi/Random.h"

namespace p3
{
class StarfieldGraphic : public Graphic
{
public:
	StarfieldGraphic(Graphics::Renderer*, Random& rng);
	virtual void Render() override;

private:
	std::unique_ptr<Graphics::VertexBuffer> m_vertexBuffer;
	Graphics::RenderState* m_renderState;
	RefCountedPtr<Graphics::Material> m_material;
};

}
