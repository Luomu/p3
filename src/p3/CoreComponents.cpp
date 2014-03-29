#include "p3/CoreComponents.h"

namespace p3
{

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
	const double fakeMax = 1e3;
	vector3d maxThrust;
	maxThrust.x = fakeMax / 2.0;
	maxThrust.y = fakeMax / 2.0;
	maxThrust.z = fakeMax;
	return maxThrust;
}

double ThrusterComponent::GetMaxAngThrust() const
{
	return 10.0;
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
