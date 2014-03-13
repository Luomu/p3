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

	vector3d   pos;
	matrix3x3d orient;

	matrix4x4d viewMatrix;

	void LookAt(vector3d eye, vector3d target, vector3d up);
};

struct CameraComponent : public entityx::Component<CameraComponent> {
	std::unique_ptr<Camera> camera;
};

//This doesn't really have to be limited to cameras!
struct CameraLookAtComponent : public entityx::Component<CameraLookAtComponent> {
	CameraLookAtComponent(Entity lookTarget) : target(lookTarget) {}
	Entity target; //should have a PosOrientComponent
};

struct AttachToEntityComponent : public entityx::Component<AttachToEntityComponent> {
	AttachToEntityComponent(Entity tgt) : target(tgt) {}
	Entity target; //should have a PosOrientComponent
};

class CameraUpdateSystem : public entityx::System<CameraUpdateSystem>
{
public:
	void update(ent_ptr<entityx::EntityManager> es, ent_ptr<entityx::EventManager> events, double dt) override;
};

}
