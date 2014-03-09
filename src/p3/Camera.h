#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"
#include "graphics/Types.h"
#include "graphics/Frustum.h"

namespace p3
{
class Camera
{
public:
	Camera();

	Graphics::Projection projection;
	Uint32 clearFlags;
	Color clearColor;
	float fovY; //degrees
	float nearZ;
	float farZ;
	vector4f viewport; //normalized viewport rectangle x,y,w,h
	Uint32 zOrder;
};

struct CameraComponent : public entityx::Component<CameraComponent> {
	std::unique_ptr<Camera> camera;
};

}
