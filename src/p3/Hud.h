#pragma once
#include "p3/Common.h"
#include "ui/Context.h"
#include "p3/EntitySystem.h"

namespace p3
{
class Hud {
public:
	Hud(UI::Context* ui, Entity observer);
	void Update(double deltaTime);

private:
	Entity m_observer;

	UI::Label* m_setSpeed;
	UI::Label* m_velocity;
	UI::Label* m_location;
};
}
