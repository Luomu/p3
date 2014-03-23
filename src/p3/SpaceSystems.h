#pragma once
#include "p3/EntitySystem.h"
#include "pi/Frame.h"
namespace p3
{

class FrameUpdateSystem : public entityx::System<FrameUpdateSystem>
{
public:
	virtual void update(ent_ptr<entityx::EntityManager> em, ent_ptr<entityx::EventManager> events, double dt) override;

private:
	void SwitchFrame(Entity e, Frame* f);
};

}
