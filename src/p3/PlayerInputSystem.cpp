#include "p3/PlayerInputSystem.h"
#include "p3/KeyBindings.h"
#include "p3/CoreComponents.h"
#include "p3/ShipAISystem.h"

namespace p3
{

void PlayerInputSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> ev, double dt)
{
	for (auto entity : em->entities_with_components<PlayerInputComponent>()) {
		ent_ptr<PlayerInputComponent> pic = entity.component<PlayerInputComponent>();
		ent_ptr<ThrusterComponent> tc     = entity.component<ThrusterComponent>();
		ent_ptr<ShipAIComponent> ai       = entity.component<ShipAIComponent>();
		ent_ptr<PosOrientComponent> poc   = entity.component<PosOrientComponent>();
		SDL_assert(tc && ai && poc);

		bool matchLinear = true;
		tc->linear  = vector3d(0.0);
		if (KeyBindings::thrustForward.IsActive()) {
			tc->linear.z = -1.0;
			matchLinear  = false;
		} else if (KeyBindings::thrustReverse.IsActive()) {
			tc->linear.z = 1.0;
			matchLinear  = false;
		}
		if (KeyBindings::thrustLeft.IsActive()) {
			tc->linear.x = -1.0;
			matchLinear  = false;
		} else if (KeyBindings::thrustRight.IsActive()) {
			tc->linear.x = 1.0;
			matchLinear  = false;
		}

		//Sticky speed: don't exceed zero, if approaching from either direction,
		//until the key is released again
		static bool stickySpeed = false;
		if (stickySpeed) {
			if (!(KeyBindings::speedIncrease.IsActive() || KeyBindings::speedDecrease.IsActive()))
				stickySpeed = false;
		}

		const double oldSpeed = pic->setSpeed;
		if (!stickySpeed) {
			if (KeyBindings::speedIncrease.IsActive())
				pic->setSpeed += 500.0 * dt;
			else if (KeyBindings::speedDecrease.IsActive())
				pic->setSpeed -= 500.0 * dt;
		}

		if ((oldSpeed > 0.0 && pic->setSpeed <= 0.0) || (oldSpeed < 0.0 && pic->setSpeed >= 0.0)) {
			pic->setSpeed = 0.0;
			stickySpeed = true;
		}

		ai->targetVelocity = -poc->orient.VectorZ() * pic->setSpeed;

		if (KeyBindings::overrideAutoVelocity.IsActive())
			matchLinear = false;

		ai->matchLinear = matchLinear;

		vector3d wantAngVel(0.0);
		if (KeyBindings::yawLeft.IsActive())
			wantAngVel.y += 1.0;
		if (KeyBindings::yawRight.IsActive())
			wantAngVel.y -= 1.0;
		if (KeyBindings::pitchDown.IsActive())
			wantAngVel.x -= 1.0;
		if (KeyBindings::pitchUp.IsActive())
			wantAngVel.x += 1.0;
		if (KeyBindings::rollLeft.IsActive())
			wantAngVel.z += 1.0;
		if (KeyBindings::rollRight.IsActive())
			wantAngVel.z -= 1.0;
		ai->targetAngVelocity = wantAngVel;

		if (wantAngVel.LengthSqr() >= 0.001)
			ai->angSoftness = 5.0;
		else
			ai->angSoftness = 1.0;

		if (KeyBindings::firePrimary.IsPressed()) {
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
		const vector3d maxthrust = tc->GetMaxThrust(tc->linear);
		dc->force += poc->orient * (tc->linear * maxthrust);

		//add rel torque
		dc->torque += poc->orient * (tc->angular * tc->GetMaxAngThrust());
	}
}

}
