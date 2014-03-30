#include "p3/ShipAISystem.h"
#include "p3/p3.h"
#include "p3/Game.h"
#include "p3/CoreComponents.h"

namespace p3
{

ShipAIComponent::ShipAIComponent()
	: matchLinear(true)
	, targetVelocity(0.0)
	, targetAngVelocity(0.0)
	, angSoftness(1.0)
	, doFaceDirection(false)
	, faceDirection(0.0)
	, faceAngVel(0.0)
{}

void ShipAIComponent::SetFaceDirection(vector3d dir, double angVel)
{
	doFaceDirection = true;
	faceDirection = dir;
	faceAngVel = angVel;
}

ShipAISystem::ShipAISystem(Sim* sim)
	: m_sim(sim)
{
}

void ShipAISystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<ShipAIComponent>()) {
		auto ai        = entity.component<ShipAIComponent>();
		auto poc       = entity.component<PosOrientComponent>();
		auto dynamics  = entity.component<DynamicsComponent>();
		auto thrusters = entity.component<ThrusterComponent>();
		auto mass      = entity.component<MassComponent>();

		if (ai->matchLinear) {
			//match velocity
			const vector3d diffvel = (ai->targetVelocity - dynamics->vel) * poc->orient;

			// ZZZ counter external forces (not yet)
			//vector3d extf = GetExternalForce() * (Pi::game->GetTimeStep() / GetMass());
			//vector3d diffvel2 = diffvel - extf * GetOrient();
			const vector3d diffvel2 = diffvel;

			//const vector3d maxThrust = GetMaxThrust(diffvel2); (no stats yet)
			const vector3d maxThrust = thrusters->GetMaxThrust(diffvel2);

			const vector3d maxFrameAccel = maxThrust * (m_sim->GetTimeStep() / mass->mass);
			const vector3d thrust(diffvel2.x / maxFrameAccel.x,
			                      diffvel2.y / maxFrameAccel.y,
			                      diffvel2.z / maxFrameAccel.z);

			thrusters->SetLinear(thrust);
		}

		//match angular velocity
		const double angInertia = dynamics->angInertia; //TODO not correct angInertia
		double angAccel = thrusters->GetMaxAngThrust() / angInertia;
		const double softTimeStep = m_sim->GetTimeStep() * ai->angSoftness;

		const vector3d angVel = ai->targetAngVelocity - dynamics->angVel * poc->orient;
		vector3d angThrust;
		for (int axis = 0; axis < 3; axis++) {
			if (angAccel * softTimeStep >= fabs(angVel[axis])) {
				angThrust[axis] = angVel[axis] / (softTimeStep * angAccel);
			} else {
				angThrust[axis] = (angVel[axis] > 0.0 ? 1.0 : -1.0);
			}
		}

		if (ai->doFaceDirection) {
			const double angInertia = dynamics->angInertia; //TODO not correct angInertia
			const double maxAccel =  thrusters->GetMaxAngThrust() / angInertia;		// should probably be in stats anyway

			vector3d head = (ai->faceDirection * poc->orient).Normalized();		// create desired object-space heading
			vector3d dav(0.0, 0.0, 0.0);	// desired angular velocity

			double ang = 0.0;
			if (head.z > -0.99999999) {
				ang = acos (Clamp(-head.z, -1.0, 1.0));		// scalar angle from head to curhead
				const double iangvel = ai->faceAngVel + CalcIvelPos(ang, 0.0, maxAccel);	// ideal angvel at current time

				// Normalize (head.x, head.y) to give desired angvel direction
				if (head.z > 0.999999) head.x = 1.0;
				const double head2dnorm = 1.0 / sqrt(head.x * head.x + head.y * head.y);		// NAN fix shouldn't be necessary if inputs are normalized
				dav.x = head.y * head2dnorm * iangvel;
				dav.y = -head.x * head2dnorm * iangvel;
			}
			const vector3d cav = dynamics->angVel * poc->orient; // current obj-rel angvel
			const double frameAccel = maxAccel * m_sim->GetTimeStep();
			angThrust = (dav - cav) / frameAccel;	// find diff between current & desired angvel

			ai->doFaceDirection = false;
		}

		thrusters->SetAngular(angThrust);
	}
}

double ShipAISystem::CalcIvelPos(double dist, double vel, double acc)
{
	double ivel = 0.9 * sqrt(vel * vel + 2.0 * acc * dist); // fudge hardly necessary

	const double endvel = ivel - (acc * m_sim->GetTimeStep());
	if (endvel <= 0.0) ivel = dist / m_sim->GetTimeStep(); // last frame discrete correction
	else ivel = (ivel + endvel) * 0.5;                     // discrete overshoot correction

	return ivel;
}

AICommandSystem::AICommandSystem()
{

}

void AICommandSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<AICommandComponent, ShipAIComponent>()) {
		auto cmdcomp = entity.component<AICommandComponent>();
		if (!cmdcomp->target.valid())
			continue;

		auto ai = entity.component<ShipAIComponent>();
		auto thrusters = entity.component<ThrusterComponent>();
		SDL_assert(thrusters);
		auto fc = entity.component<FrameComponent>();
		SDL_assert(fc);
		auto dc = entity.component<DynamicsComponent>();

		//hardcoded kamikaze behavior
		const vector3d targetPos = Space::GetPosRelTo(cmdcomp->target, entity);
		const vector3d targetDir = targetPos.NormalizedSafe();
		const double dist = targetPos.Length();

		// Don't come in too fast when we're close, so we don't overshoot by
		// too much if we miss the target.

		// Aim to collide at a speed which would take us 4s to reverse.
		const double aimCollisionSpeed = thrusters->GetAccelFwd() * 2.0;

		// Aim to use 1/4 of our acceleration for braking while closing
		// distance, leaving the rest for course adjustment.
		const double brake = thrusters->GetAccelFwd() * 0.25;

		const double aimRelSpeed =
		    sqrt(aimCollisionSpeed * aimCollisionSpeed + 2.0 * dist * brake);

		const vector3d aimVel = aimRelSpeed * targetDir + Space::GetVelRelTo(cmdcomp->target, fc->frame);
		const vector3d accelDir = (aimVel - dc->vel).NormalizedSafe();

		ai->SetFaceDirection(accelDir);
	}
}

}
