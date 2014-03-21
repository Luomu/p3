#pragma once
#include "p3/EntitySystem.h"
#include "p3/Common.h"

namespace p3
{

class Sim;

struct ShipAIComponent : public entityx::Component<ShipAIComponent> {
	ShipAIComponent();

	bool matchLinear;
	vector3d targetVelocity;
	vector3d targetAngVelocity;
	double angSoftness;
};

/**
 * Controls orientation and velocity using Thrusters. Also requires:
 *  - PosOrient
 *  - Dynamics
 *  - Frame
 */
class ShipAISystem : public entityx::System<ShipAISystem>
{
public:
	ShipAISystem(Sim* sim);
	virtual void update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt) override;

private:
	Sim* m_sim;
};

}
