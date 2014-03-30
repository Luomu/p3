#include "p3/CoreComponents.h"

namespace p3
{

const double PLACEHOLDER_MASS = 10.0;
const double PLACEHOLDER_THRUST = 1e3;

PosOrientComponent::PosOrientComponent()
	: pos(0.0), orient(1.0)
	//,interpPos(0.0), interpOrient(1.0)
	//, oldPos(0.0), oldAngDisplacement(0.0)
{}

PosOrientComponent::PosOrientComponent(vector3d initialPos, const matrix3x3d& initialOrient)
	: pos(initialPos), orient(initialOrient)
	//, interpPos(initialPos), interpOrient(initialOrient), oldPos(initialPos), oldAngDisplacement(0.0)
{}

vector3d ThrusterComponent::GetMaxThrust(vector3d v) const
{
	vector3d maxThrust;
	maxThrust.x = PLACEHOLDER_THRUST / 2.0;
	maxThrust.y = PLACEHOLDER_THRUST / 2.0;
	maxThrust.z = PLACEHOLDER_THRUST;
	return maxThrust;
}

double ThrusterComponent::GetMaxAngThrust() const
{
	return 10.0;
}

double ThrusterComponent::GetAccelFwd() const
{
	return PLACEHOLDER_THRUST / PLACEHOLDER_MASS;
}

void ThrusterComponent::SetLinear(vector3d v)
{
	linear.x = Clamp(v.x, -1.0, 1.0);
	linear.y = Clamp(v.y, -1.0, 1.0);
	linear.z = Clamp(v.z, -1.0, 1.0);
}

void ThrusterComponent::SetAngular(vector3d v)
{
	angular.x = Clamp(v.x, -1.0, 1.0);
	angular.y = Clamp(v.y, -1.0, 1.0);
	angular.z = Clamp(v.z, -1.0, 1.0);
}

}
