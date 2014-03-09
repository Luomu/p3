#include "p3/Camera.h"

namespace p3
{
Camera::Camera()
	: projection(Graphics::Projection::PERSPECTIVE)
	, clearFlags(500) //ZZZ assuming clear all
	, clearColor(12, 12, 93, 0)
	, fovY(60.f)
	, nearZ(1.f)
	, farZ(10000.f)
	, viewport(0.f, 0.f, 1.f, 1.f)
	, zOrder(0)
{

}
}
