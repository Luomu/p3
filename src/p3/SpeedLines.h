#pragma once
#include "p3/Graphic.h"
#include "p3/EntitySystem.h"

namespace p3
{

class SpeedLines : public Graphic
{
public:
	SpeedLines(Graphics::Renderer*, Entity owner);
	void Update(double deltaTime);
	virtual void Render() override;

private:
	Graphics::RenderState* m_renderState;
	std::vector<vector3f> m_points;
	std::vector<vector3f> m_vertices;
	std::vector<Color> m_vtxColors;
	Color m_color;
	bool m_visible;
	float m_lineLength;
	vector3f m_dir;
	matrix4x4d m_transform;
	Entity m_owner;
};

}
