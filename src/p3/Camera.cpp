#include "p3/Camera.h"
#include "p3/CoreComponents.h"

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

void CameraUpdateSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	//copy interp orient/pos to camera pos/orient (BLEARGH)
	for (auto entity : em->entities_with_components<CameraComponent>()) {
		ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();
        ent_ptr<CameraComponent> cc = entity.component<CameraComponent>();
        SDL_assert(cc);
        cc->camera->pos = poc->pos;
        cc->camera->orient = poc->orient;
	}
}

}
