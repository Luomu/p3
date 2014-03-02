#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"

namespace p3
{

struct DynamicsComponent : public entityx::Component<DynamicsComponent> {
	double mass;
	vector3d force;
	vector3d torque;
	vector3d vel;
	vector3d angVel;
};
}
