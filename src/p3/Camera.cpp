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

void CameraUpdateSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<CameraComponent>()) {
		ent_ptr<CameraComponent> cc = entity.component<CameraComponent>();
		ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();
		SDL_assert(poc);
		auto clc = entity.component<CameraLookAtComponent>();
		auto att = entity.component<AttachToEntityComponent>();

		if (att) {
			auto tgtPoc = att->target.component<PosOrientComponent>();
			SDL_assert(tgtPoc);
			poc->pos = tgtPoc->pos + tgtPoc->orient * att->offset;
			poc->orient = tgtPoc->orient;
		}

		if (clc) {
			auto tgtPoc = clc->target.component<PosOrientComponent>();
			SDL_assert(tgtPoc);
			cc->camera->viewMatrix = matrix4x4d::LookAt(poc->pos, tgtPoc->pos, vector3d(0, 1, 0));

			const vector3d tgt = tgtPoc->pos;
			const vector3d eye = poc->pos;
			const vector3d up  = vector3d(0,1,0);

			const vector3d zaxis = (tgt - eye).Normalized();
			vector3d yaxis = up.Normalized();
			const vector3d xaxis = zaxis.Cross(yaxis).Normalized();
			yaxis = xaxis.Cross(zaxis);

			poc->orient = matrix3x3d::FromVectors(xaxis, yaxis, -zaxis);
		} else {
			matrix4x4d vmd = poc->orient;
			vmd.SetTranslate(poc->pos);
			cc->camera->viewMatrix = vmd.InverseOf();
		}
	}
}

}
