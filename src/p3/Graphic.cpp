#include "p3/Graphic.h"
namespace p3
{
void ModelGraphic::Render()
{
	matrix4x4f viewMatrix(1.f);
	viewMatrix.SetTranslate(vector3f(0.f, 0.f, -100.f));

	model->Render(viewMatrix * modelTransform);
}
}
