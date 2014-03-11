#include "p3/Graphic.h"
namespace p3
{
void ModelGraphic::Render()
{
	static matrix4x4f vmf;
	matrix4x4dtof(m_renderer->GetViewMatrix() * modelTransform, vmf);
	model->Render(vmf);
}
}
