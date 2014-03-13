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
	, pos(0.0)
	, orient(1.0)
	, viewMatrix(1.0)
{
}

void Camera::LookAt(const vector3d eye, const vector3d tgt, const vector3d up)
{
	const vector3d zaxis = (eye - tgt).Normalized();
	const vector3d xaxis = (up.Cross(zaxis)).Normalized();
	const vector3d yaxis = zaxis.Cross(xaxis);

	orient = matrix3x3d::FromVectors(xaxis, yaxis, zaxis);

	pos.x = -xaxis.Dot(eye);
	pos.y = -yaxis.Dot(eye);
	pos.z = -zaxis.Dot(eye);
}

void CameraUpdateSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	//copy interp orient/pos to camera pos/orient (BLEARGH)
	for (auto entity : em->entities_with_components<CameraComponent>()) {
		ent_ptr<CameraComponent> cc = entity.component<CameraComponent>();
		ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();
		SDL_assert(poc);
		auto clc = entity.component<CameraLookAtComponent>();
		auto att = entity.component<AttachToEntityComponent>();
		if (clc) {
			auto tgtPoc = clc->target.component<PosOrientComponent>();
			SDL_assert(tgtPoc);
			cc->camera->LookAt(poc->pos, tgtPoc->pos, vector3d(0, 1, 0));

			matrix4x4d vmd = cc->camera->orient.Transpose();
			vmd.SetTranslate(cc->camera->pos);
			cc->camera->viewMatrix = vmd;
		} else if (att) {
			auto tgtPoc = att->target.component<PosOrientComponent>();
			SDL_assert(tgtPoc);
			cc->camera->pos = tgtPoc->pos + tgtPoc->orient * vector3d(0, 5, 30);
			cc->camera->orient = tgtPoc->orient;

			matrix4x4d vmd = cc->camera->orient;
			vmd.SetTranslate(cc->camera->pos);
			cc->camera->viewMatrix = vmd.InverseOf();
		} else {
			cc->camera->pos = poc->pos;
			cc->camera->orient = poc->orient;

			matrix4x4d vmd = cc->camera->orient;
			vmd.SetTranslate(cc->camera->pos);
			cc->camera->viewMatrix = vmd.InverseOf();
		}
	}
}

}
