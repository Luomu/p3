#include "p3/DynamicsSystem.h"
#include "p3/CoreComponents.h"

namespace p3
{

void DynamicsSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<DynamicsComponent>()) {
        ent_ptr<DynamicsComponent> dc = entity.component<DynamicsComponent>();
        ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();
        ent_ptr<MassComponent> mc = entity.component<MassComponent>();
        SDL_assert(poc);
        SDL_assert(mc);

        //save previous
        poc->oldPos = poc->pos;
        poc->oldAngDisplacement = dc->angVel * dt;

        dc->force += dc->externalForce;

		dc->vel += dt * dc->force * (1.0 / mc->mass);
		dc->angVel += dt * dc->torque * (1.0 / dc->angInertia);

		//update pos
		poc->pos += dc->vel * dt;
		//update orient
		double len = dc->angVel.Length();
		if (len > 1e-16) {
			vector3d axis = dc->angVel * (1.0 / len);
			matrix3x3d r = matrix3x3d::Rotate(len * dt, axis);
			poc->orient = r * poc->orient;
		}

		dc->force = vector3d(0.0);
		dc->torque = vector3d(0.0);

		//calculate external forces
		//- gravity
		//- atmospheric drag
		//- centrifugal/coriolis force
        auto fc = entity.component<FrameComponent>();
        if (fc) {
			if (fc->frame->IsRotFrame()) {
				vector3d angRot(0, fc->frame->GetAngSpeed(), 0);
				dc->externalForce -= mc->mass * angRot.Cross(angRot.Cross(poc->pos));	// centrifugal
				dc->externalForce -= 2 * mc->mass * angRot.Cross(dc->vel);			// coriolis
			}
        }
	}
}

void TransInterpSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double alpha)
{
	for (auto entity : em->entities_with_components<PosOrientComponent>()) {
		ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();

		poc->interpPos = alpha * poc->pos + (1.0 - alpha) * poc->oldPos;

		const double len = poc->oldAngDisplacement.Length() * (1.0 - alpha);
		if (len > 1e-16) {
			const vector3d axis = poc->oldAngDisplacement.Normalized();
			const matrix3x3d rot = matrix3x3d::Rotate(-len, axis); //rotate backwards
			poc->interpOrient = rot * poc->orient;
		} else
			poc->interpOrient = poc->orient;
	}
}

}
