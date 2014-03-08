#pragma once
#include "p3/EntitySystem.h"

namespace p3
{

//update thrusters from inputs
class PlayerInputSystem : public entityx::System<PlayerInputSystem>
{
public:
	void update(ent_ptr<EntityManager> es, ent_ptr<EventManager> events, double dt) override;
};

//update dynamics parameters from thrusters (not player specific)
class ThrusterSystem : public entityx::System<ThrusterSystem>
{
public:
	void update(ent_ptr<EntityManager> es, ent_ptr<EventManager> events, double dt) override;
};

}
