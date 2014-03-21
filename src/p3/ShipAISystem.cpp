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
	{}

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

		if (ai->matchLinear)
		{
			//match velocity
			const vector3d diffvel = (ai->targetVelocity - dynamics->vel) * poc->orient;

			// counter external forces (not yet)
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

		thrusters->SetAngular(angThrust);
	}
}

}
