#pragma once
#include "p3/EntitySystem.h"
#include "p3/Common.h"

namespace p3
{

class Sim;

struct ShipAIComponent : public entityx::Component<ShipAIComponent> {
	ShipAIComponent();

	void SetFaceDirection(vector3d dir, double angVel = 0.0); //frame-relative direction

	bool matchLinear;
	vector3d targetVelocity;
	vector3d targetAngVelocity;
	double angSoftness;

	bool doFaceDirection;
	vector3d faceDirection;
	double faceAngVel;
};

/**
 * Gives instructions to ShipAIComponent
 */
struct AICommandComponent : public entityx::Component<AICommandComponent> {
	void SetKamikaze(Entity tgt) { target = tgt; }
	Entity target;
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
	double CalcIvelPos(double dist, double vel, double acc);
	Sim* m_sim;
};

class AICommandSystem : public entityx::System<AICommandSystem>
{
public:
	AICommandSystem();
	virtual void update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt) override;
};

}
