#include "p3/PlayerInputSystem.h"
#include "p3/KeyBindings.h"
#include "p3/CoreComponents.h"

namespace p3
{

void PlayerInputSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> ev, double dt)
{
	for (auto entity : em->entities_with_components<PlayerInputComponent>()) {
		ent_ptr<ThrusterComponent> tc = entity.component<ThrusterComponent>();
		SDL_assert(tc);
		tc->linear  = vector3d(0.0);
		tc->angular = vector3d(0.0);
		if (KeyBindings::thrustIncrease.IsActive()) tc->linear.z -= 100.0;
		if (KeyBindings::thrustDecrease.IsActive()) tc->linear.z += 100.0;

		vector3d wantAngVel(0.0);
		if (KeyBindings::yawLeft.IsActive())   wantAngVel.y += 1.0;
		if (KeyBindings::yawRight.IsActive())  wantAngVel.y -= 1.0;
		if (KeyBindings::pitchDown.IsActive()) wantAngVel.x -= 1.0;
		if (KeyBindings::pitchUp.IsActive())   wantAngVel.x += 1.0;
		if (KeyBindings::rollLeft.IsActive())  wantAngVel.z += 1.0;
		if (KeyBindings::rollRight.IsActive()) wantAngVel.z -= 1.0;
		tc->angular += wantAngVel;

		if (KeyBindings::firePrimary.IsPressed())
		{
			ent_ptr<WeaponComponent> wc = entity.component<WeaponComponent>();
			SDL_assert(wc);
			wc->firing = true;
		}
	}
}

void ThrusterSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> ev, double dt)
{
	for (auto entity : em->entities_with_components<ThrusterComponent>()) {
		ent_ptr<ThrusterComponent> tc   = entity.component<ThrusterComponent>();
		ent_ptr<DynamicsComponent> dc   = entity.component<DynamicsComponent>();
		ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>(); //for orient
		SDL_assert(dc);
		SDL_assert(poc);

		//add rel force
		dc->force += poc->orient * tc->linear;

		//add rel torque
		dc->torque += poc->orient * tc->angular;
	}
}

}
