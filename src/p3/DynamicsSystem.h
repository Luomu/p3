#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"

namespace p3
{

class DynamicsSystem : public entityx::System<DynamicsSystem>
{
public:
	void update(ent_ptr<entityx::EntityManager> es, ent_ptr<entityx::EventManager> events, double dt) override;
};
}
